#include "OpenCVSimplification.h"
#include "ContourUtils.h"

namespace simp {

	OpenCVSimplification::OpenCVSimplification(const std::vector<cv::Mat>& voxel_data, double epsilon, double layering_threshold, double snap_vertex_threshold, double snap_edge_threshold) {
		this->voxel_data = voxel_data;
		this->epsilon = epsilon;
		this->layering_threshold = layering_threshold;
		this->snap_vertex_threshold = snap_vertex_threshold;
		this->snap_edge_threshold = snap_edge_threshold;
		this->size = cv::Size(voxel_data[0].cols, voxel_data[0].rows);
	}

	void OpenCVSimplification::simplify(std::vector<Building>& buildings) {
		buildings.clear();

		std::vector<util::Polygon> polygons = util::findContours(voxel_data[5]);
		for (int i = 0; i < polygons.size(); i++) {
			calculateBuilding(NULL, polygons[i], 5, buildings);
		}
	}

	void OpenCVSimplification::calculateBuilding(Building* parent, const util::Polygon& polygon, int height, std::vector<Building>& buildings) {
		// calculate the bounding box
		cv::Rect bbox = util::boundingBox(polygon.contour);

		// have 1px as margin
		bbox.x = std::max(0, bbox.x - 1);
		bbox.y = std::max(0, bbox.y - 1);
		bbox.width = std::min(size.width - bbox.x - 1, bbox.width + 2);
		bbox.height = std::min(size.height - bbox.y - 1, bbox.height + 2);

		// find the height at which the contour drastically changes
		int next_height = findDrasticChange(height, polygon, layering_threshold);
		
		// calculate building by simplifying the contour and holes
		try {
			Building building = calculateBuildingComponent(parent, polygon, height, next_height);
			buildings.push_back(building);
			
			if (next_height >= voxel_data.size()) return;

			// extract contours
			std::vector<util::Polygon> polygons = util::findContours(cv::Mat(voxel_data[next_height], bbox));

			for (int i = 0; i < polygons.size(); i++) {
				// offset back the contour and holes
				polygons[i].translate(bbox.x, bbox.y);

				// check if the next contour is mostly within the contour
				int cnt_total = 0;
				int cnt_outside = 0;
				cv::Rect next_bbox = util::boundingBox(polygons[i].contour);
				for (int j = 0; j < 100; j++) {
					cv::Point pt;
					bool found = false;
					for (int k = 0; k < 100; k++) {
						pt = cv::Point(rand() % next_bbox.width + next_bbox.x, rand() % next_bbox.height + next_bbox.y);
						if (pointPolygonTest(polygons[i].contour, pt, false) >= 0) {
							found = true;
							break;
						}
					}

					if (found) {
						cnt_total++;
						if (pointPolygonTest(polygon.contour, pt, false) < 0) cnt_outside++;
					}
				}

				if (cnt_outside < cnt_total / 2) {
					// for upper layers, recursive call this function to construct building components
					calculateBuilding(&building, polygons[i], next_height, buildings);
				}
			}
		}
		catch (const char* ex) {
		}
	}

	/**
	* Calculate the building geometry by simplifying the specified footprint and holes using OpenCV function.
	*/
	Building OpenCVSimplification::calculateBuildingComponent(Building* parent, const util::Polygon& polygon, int bottom_height, int top_height) {
		// calculate the bounding box
		cv::Rect bbox = util::boundingBox(polygon.contour);

		// have 1px as margin
		bbox.x = std::max(0, bbox.x - 1);
		bbox.y = std::max(0, bbox.y - 1);
		bbox.width = std::min(size.width - bbox.x - 1, bbox.width + 2);
		bbox.height = std::min(size.height - bbox.y - 1, bbox.height + 2);

		// select the best slice that has the best IOU with all the slices in the layer
		double best_iou = 0;
		int best_slice = -1;
		for (int i = bottom_height; i < top_height; i++) {
			double iou = 0;
			for (int j = bottom_height; j < top_height; j++) {
				// calculate IOU
				iou += util::calculateIOU(voxel_data[i], voxel_data[j], bbox);
			}

			if (iou > best_iou) {
				best_iou = iou;
				best_slice = i;
			}
		}
		
		// extract contours in the specified region
		std::vector<util::Polygon> polygons = util::findContours(cv::Mat(voxel_data[best_slice], bbox));
		if (polygons.size() == 0) throw "No building is found.";
		
		// We should check which contour is the one to be processed,
		// but we use the first one for now.
		polygons[0].translate(bbox.x, bbox.y);
		util::Polygon simplified_polygon = simplifyPolygon(polygons[0], epsilon);
		if (simplified_polygon.contour.size() < 3) throw "Invalid contour";
		
		// create a building object
		Building building;
		building.bottom_height = bottom_height;
		building.top_height = top_height;

		building.footprint.resize(simplified_polygon.contour.size());
		for (int i = 0; i < simplified_polygon.contour.size(); i++) {
			building.footprint[i] = cv::Point2f(simplified_polygon.contour[i].x - size.width * 0.5, size.height * 0.5 - simplified_polygon.contour[i].y);
		}

		if (parent != NULL) {
			util::snapPolygon(parent->footprint, building.footprint, snap_vertex_threshold, snap_edge_threshold);
		}

		// simplify the hole as well
		building.holes.resize(simplified_polygon.holes.size());
		for (int i = 0; i < simplified_polygon.holes.size(); i++) {
			building.holes[i].resize(simplified_polygon.holes[i].size());
			for (int j = 0; j < simplified_polygon.holes[i].size(); j++) {
				building.holes[i][j] = cv::Point2f(simplified_polygon.holes[i][j].x - size.width * 0.5, size.height * 0.5 - simplified_polygon.holes[i][j].y);
			}
		}

		return building;
	}

	int OpenCVSimplification::findDrasticChange(int height, const util::Polygon& polygon, double threshold) {
		// calculate the bounding box
		cv::Rect bbox = util::boundingBox(polygon.contour);

		for (int i = height + 1; i < voxel_data.size(); i++) {
			double iou = util::calculateIOU(voxel_data[height], voxel_data[i], bbox);
			if (iou < threshold) {
				return i;
			}
		}

		return voxel_data.size();
	}

	util::Polygon OpenCVSimplification::simplifyPolygon(const util::Polygon& polygon, double epsilon) {
		util::Polygon ans;
		cv::approxPolyDP(polygon.contour, ans.contour, epsilon, true);
	
		// simplify the hole as well
		for (int i = 0; i < polygon.holes.size(); i++) {
			std::vector<cv::Point> simplified_hole;
			cv::approxPolyDP(polygon.holes[i], simplified_hole, epsilon, true);
			if (simplified_hole.size() >= 3) {
				ans.holes.push_back(simplified_hole);
			}
		}

		// ToDo:
		// Should we check if the holes are inside the contour?

		return ans;
	}

}