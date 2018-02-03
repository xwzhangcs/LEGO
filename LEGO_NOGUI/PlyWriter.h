#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Partition_traits_2.h>
#include <CGAL/partition_2.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/random_polygon_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/create_offset_polygons_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/random_polygon_2.h>
#include <CGAL/Polygon_2.h>
#include "Building.h"

namespace util {

	namespace ply {

		// The following definitions are for triangulation only.
		struct FaceInfo {
			FaceInfo() {}
			int nesting_level;
			bool in_domain(){
				return nesting_level % 2 == 1;
			}
		};

		typedef CGAL::Exact_predicates_inexact_constructions_kernel       K;
		typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
		typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo, K>    Fbb;
		typedef CGAL::Constrained_triangulation_face_base_2<K, Fbb>        Fb;
		typedef CGAL::Triangulation_data_structure_2<Vb, Fb>               TDS;
		typedef CGAL::Exact_predicates_tag                                Itag;
		typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>  CDT;
		typedef CDT::Point                                                Point;
		typedef CGAL::Partition_traits_2<K>                         Traits;
		typedef Traits::Polygon_2                                   Polygon_2;
		typedef Traits::Point_2                                     Point_2;
		typedef Polygon_2::Vertex_iterator                          Vertex_iterator;
		typedef std::list<Polygon_2>                                Polygon_list;
		typedef CGAL::Creator_uniform_2<int, Point_2>               Creator;
		typedef CGAL::Random_points_in_square_2< Point_2, Creator > Point_generator;
		typedef boost::shared_ptr<Polygon_2>						PolygonPtr;

		class Point3d {
		public:
			double x;
			double y;
			double z;

		public:
			Point3d(double x, double y, double z) : x(x), y(y), z(z) {}
			friend bool operator<(const Point3d& p1, const Point3d& p2) {
				return std::tie(p1.x, p1.y, p1.z) < std::tie(p2.x, p2.y, p2.z);
			}
		};


		class PlyWriter {
		protected:
			PlyWriter() {}

		public:
			static void write(const char* filename, const std::vector<std::shared_ptr<simp::Building>>& buildings);

		private:
			static void writeBuilding(std::shared_ptr<simp::Building> building, std::map<Point3d, int>& vertices_map, std::vector<Point3d>& vertices, std::vector<std::vector<int>>& faces);

			static std::vector<std::vector<cv::Point2f>> tessellate(const std::vector<cv::Point2f>& points);
			static std::vector<std::vector<cv::Point2f>> tessellate(const std::vector<cv::Point2f>& points, const std::vector<std::vector<cv::Point2f>>& holes);
			static void mark_domains(CDT& ct, CDT::Face_handle start, int index, std::list<CDT::Edge>& border);
			static void mark_domains(CDT& cdt);
		};

	}
}