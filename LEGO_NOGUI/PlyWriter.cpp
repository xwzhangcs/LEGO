#include "PlyWriter.h"
#include <fstream>
#include "ContourUtils.h"

namespace util {
	
	namespace ply {

		void PlyWriter::write(const char* filename, const std::vector<std::shared_ptr<simp::Building>>& buildings) {
			std::map<Point3d, int> vertices_map;
			std::vector<Point3d> vertices;
			std::vector<std::vector<int>> faces;

			for (int i = 0; i < buildings.size(); i++) {
				writeBuilding(buildings[i], vertices_map, vertices, faces);
			}

			std::ofstream out(filename);
			out << "ply" << std::endl;
			out << "format ascii 1.0" << std::endl;
			out << "element vertex " << vertices.size() << std::endl;
			out << "property double x" << std::endl;
			out << "property double y" << std::endl;
			out << "property double z" << std::endl;
			out << "element face " << faces.size() << std::endl;
			out << "property list uchar int vertex_indices" << std::endl;
			out << "end_header" << std::endl;
			
			// write vertices
			for (int i = 0; i < vertices.size(); i++) {
				out << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << std::endl;
			}

			// write faces
			for (int i = 0; i < faces.size(); i++) {
				out << faces[i].size();
				for (int j = 0; j < faces[i].size(); j++) {
					out << " " << faces[i][j];
				}
				out << std::endl;
			}

			out.close();
		}

		void PlyWriter::writeBuilding(std::shared_ptr<simp::Building> building, std::map<Point3d, int>& vertices_map, std::vector<Point3d>& vertices, std::vector<std::vector<int>>& faces) {
			std::vector<std::vector<cv::Point2f>> polygons;

			if (building->footprint.holes.size() == 0) {
				polygons = tessellate(building->footprint.contour);
			}
			else {
				polygons = tessellate(building->footprint.contour, building->footprint.holes);
			}

			for (auto polygon : polygons) {
				util::clockwise(polygon);

				std::vector<int> bottom_face(polygon.size());
				std::vector<int> top_face(polygon.size());

				for (int i = 0; i < polygon.size(); i++) {
					Point3d pt_bottom(polygon[i].x, polygon[i].y, building->bottom_height);
					Point3d pt_top(polygon[i].x, polygon[i].y, building->top_height);

					int index_bottom;
					if (vertices_map.find(pt_bottom) == vertices_map.end()) {
						index_bottom = vertices.size();
						vertices.push_back(pt_bottom);
						vertices_map[pt_bottom] = index_bottom;
					}
					else {
						index_bottom = vertices_map[pt_bottom];
					}
					bottom_face[i] = index_bottom;

					int index_top;
					if (vertices_map.find(pt_top) == vertices_map.end()) {
						index_top = vertices.size();
						vertices.push_back(pt_top);
						vertices_map[pt_top] = index_top;
					}
					else {
						index_top = vertices_map[pt_top];
					}
					top_face[polygon.size() - i - 1] = index_top;
				}

				faces.push_back(bottom_face);
				faces.push_back(top_face);
			}
			
			// side faces
			std::vector<cv::Point2f> polygon = building->footprint.contour;
			util::counterClockwise(polygon);
			for (int i = 0; i < polygon.size(); i++) {
				int next = (i + 1) % polygon.size();
				Point3d p1(polygon[i].x, polygon[i].y, building->bottom_height);
				Point3d p2(polygon[next].x, polygon[next].y, building->bottom_height);
				Point3d p3(polygon[next].x, polygon[next].y, building->top_height);
				Point3d p4(polygon[i].x, polygon[i].y, building->top_height);

				faces.push_back({ vertices_map[p1], vertices_map[p2], vertices_map[p3], vertices_map[p4] });
			}

			// side faces of holes
			for (auto hole : building->footprint.holes) {
				util::clockwise(hole);
				for (int i = 0; i < hole.size(); i++) {
					int next = (i + 1) % hole.size();
					Point3d p1(hole[i].x, hole[i].y, building->bottom_height);
					Point3d p2(hole[next].x, hole[next].y, building->bottom_height);
					Point3d p3(hole[next].x, hole[next].y, building->top_height);
					Point3d p4(hole[i].x, hole[i].y, building->top_height);

					faces.push_back({ vertices_map[p1], vertices_map[p2], vertices_map[p3], vertices_map[p4] });
				}
			}

			for (int i = 0; i < building->children.size(); i++) {
				writeBuilding(building->children[i], vertices_map, vertices, faces);
			}
		}

		std::vector<std::vector<cv::Point2f>> PlyWriter::tessellate(const std::vector<cv::Point2f>& points) {
			std::vector<std::vector<cv::Point2f>> ans;

			Polygon_2 polygon;
			for (int i = 0; i < points.size(); ++i) {
				polygon.push_back(Point_2(points[i].x, points[i].y));
			}

			if (polygon.is_clockwise_oriented()) {
				polygon.reverse_orientation();
			}

			// tesselate the concave polygon
			if (polygon.is_simple()) {
				Polygon_list partition_polys;
				Traits       partition_traits;
				CGAL::greene_approx_convex_partition_2(polygon.vertices_begin(), polygon.vertices_end(), std::back_inserter(partition_polys), partition_traits);

				for (auto fit = partition_polys.begin(); fit != partition_polys.end(); ++fit) {
					std::vector<cv::Point2f> pol;
					for (auto vit = fit->vertices_begin(); vit != fit->vertices_end(); ++vit) {
						pol.push_back(cv::Point2f(vit->x(), vit->y()));
					}

					util::counterClockwise(pol);
					ans.push_back(pol);
				}
			}

			return ans;
		}

		std::vector<std::vector<cv::Point2f>> PlyWriter::tessellate(const std::vector<cv::Point2f>& points, const std::vector<std::vector<cv::Point2f>>& holes) {
			std::vector<std::vector<cv::Point2f>> ans;

			//Insert the polygons into a constrained triangulation
			CDT cdt;
			Polygon_2 polygon;
			for (int i = 0; i < points.size(); i++) {
				polygon.push_back(Point(points[i].x, points[i].y));
			}

			if (polygon.is_simple()) {
				cdt.insert_constraint(polygon.vertices_begin(), polygon.vertices_end(), true);
				for (int i = 0; i < holes.size(); i++) {
					Polygon_2 polygon;
					for (int j = 0; j < holes[i].size(); j++) {
						polygon.push_back(Point(holes[i][j].x, holes[i][j].y));
					}
					cdt.insert_constraint(polygon.vertices_begin(), polygon.vertices_end(), true);
				}

				//Mark facets that are inside the domain bounded by the polygon
				mark_domains(cdt);

				for (CDT::Finite_faces_iterator fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
					if (fit->info().in_domain()) {
						std::vector<cv::Point2f> pol;
						for (int i = 0; i < 3; i++) {
							CDT::Vertex_handle vh = fit->vertex(i);
							pol.push_back(cv::Point2f(vh->point().x(), vh->point().y()));
						}

						util::counterClockwise(pol);
						ans.push_back(pol);
					}
				}
			}

			return ans;
		}

		void PlyWriter::mark_domains(CDT& ct, CDT::Face_handle start, int index, std::list<CDT::Edge>& border) {
			if (start->info().nesting_level != -1){
				return;
			}
			std::list<CDT::Face_handle> queue;
			queue.push_back(start);
			while (!queue.empty()){
				CDT::Face_handle fh = queue.front();
				queue.pop_front();
				if (fh->info().nesting_level == -1){
					fh->info().nesting_level = index;
					for (int i = 0; i < 3; i++){
						CDT::Edge e(fh, i);
						CDT::Face_handle n = fh->neighbor(i);
						if (n->info().nesting_level == -1){
							if (ct.is_constrained(e)) {
								border.push_back(e);


							}
							else queue.push_back(n);
						}
					}
				}
			}
		}

		//explore set of facets connected with non constrained edges,
		//and attribute to each such set a nesting level.
		//We start from facets incident to the infinite vertex, with a nesting
		//level of 0. Then we recursively consider the non-explored facets incident 
		//to constrained edges bounding the former set and increase the nesting level by 1.
		//Facets in the domain are those with an odd nesting level.
		void PlyWriter::mark_domains(CDT& cdt) {
			for (CDT::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); ++it){
				it->info().nesting_level = -1;
			}
			std::list<CDT::Edge> border;
			mark_domains(cdt, cdt.infinite_face(), 0, border);
			while (!border.empty()){
				CDT::Edge e = border.front();
				border.pop_front();
				CDT::Face_handle n = e.first->neighbor(e.second);
				if (n->info().nesting_level == -1){
					mark_domains(cdt, n, e.first->info().nesting_level + 1, border);
				}
			}
		}

	}

}