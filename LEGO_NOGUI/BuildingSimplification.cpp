#include "BuildingSimplification.h"
#include "OpenCVSimplification.h"
#include "ContourUtils.h"
#include "LayerVoxelData.h"
#include "OpenCVSimplification.h"
#include "OurCustomSimplification.h"

namespace simp {

	std::vector<std::shared_ptr<Building>> BuildingSimplification::simplifyBuildings(const util::DisjointVoxelData& disjointed_voxel_data, int algorithm, float layering_threshold, float snap_vertex_threshold, float snap_edge_threshold, int epsilon, int resolution, float curve_threshold) {
		std::vector<std::shared_ptr<simp::Building>> buildings;
		for (int i = 0; i < disjointed_voxel_data.size(); i++) {
			try {
				std::vector<cv::Mat_<uchar>> voxel_data = disjointed_voxel_data.getDisjointedVoxelData(i);

				util::LayerVoxelData lvd(voxel_data, 0.5);

				// obtain the XY dimension of the voxel data
				cv::Size size;
				if (voxel_data.size() > 0) {
					size = cv::Size(voxel_data[0].cols, voxel_data[0].rows);
				}

				std::shared_ptr<util::Layer> layer = lvd.layering(layering_threshold);

				std::shared_ptr<Building> building;
				if (algorithm == ALG_ALL) {
					building = simplifyBuildingByAll(size, layer);
				}
				else if (algorithm == ALG_OPENCV) {
					building = simplifyBuildingByOpenCV(size, layer, snap_vertex_threshold, snap_edge_threshold, epsilon);
				}
				else if (algorithm == ALG_RIGHTANGLE) {
					float angle = -1;
					int dx = -1;
					int dy = -1;
					building = simplifyBuildingByOurCustom(size, layer, snap_vertex_threshold, snap_edge_threshold, resolution, angle, dx, dy);
				}
				else if (algorithm == ALG_CURVE) {
					building = simplifyBuildingByCurve(size, layer, snap_vertex_threshold, snap_edge_threshold, epsilon, curve_threshold);
				}

				buildings.push_back(building);
			}
			catch (...) {}
		}

		return buildings;
	}

	std::shared_ptr<Building> BuildingSimplification::simplifyBuildingByAll(const cv::Size& size, std::shared_ptr<util::Layer> layer) {
		std::vector<util::Polygon> polygons = util::findContours(layer->slices[0]);

		if (polygons.size() == 0) throw "No building voxel is found in this layer.";

		float best_cost = std::numeric_limits<float>::max();
		util::Polygon best_simplified_polygon;
		bool found_best = false;

		// try OpenCV
		try {
			util::Polygon simplified_polygon = OpenCVSimplification::simplify(layer->selectRepresentativeSlice(), 1);
			float cost = calculateCost(simplified_polygon, layer);
			if (cost < best_cost) {
				found_best = true;
				best_cost = cost;
				best_simplified_polygon = simplified_polygon;
			}
		}
		catch (...) {
		}

		// try right angle
		try {
			float angle = -1;
			int dx = -1;
			int dy = -1;
			util::Polygon simplified_polygon = OurCustomSimplification::simplify(layer->selectRepresentativeSlice(), 4, angle, dx, dy);
			float cost = calculateCost(simplified_polygon, layer);
			if (cost < best_cost) {
				found_best = true;
				best_cost = cost;
				best_simplified_polygon = simplified_polygon;
			}
		}
		catch (...) {
		}

		// try curve
		// ...

		if (!found_best) throw "No valid simplification is found.";

		cv::Mat_<float> mat = (cv::Mat_<float>(3, 3) << 1, 0, -size.width / 2, 0, -1, size.height / 2, 0, 0, 1);
		best_simplified_polygon.transform(mat);
		std::shared_ptr<Building> building = std::shared_ptr<Building>(new Building(best_simplified_polygon, layer->bottom_height, layer->top_height));

		for (int i = 0; i < layer->children.size(); i++) {
			try {
				std::shared_ptr<Building> child = simplifyBuildingByAll(size, layer->children[i]);
				building->children.push_back(child);
			}
			catch (...) {
			}
		}

		return building;
	}

	/**
	 * Simplify the shape of a building.
	 *
	 * @param layer		layer
	 * @param epsilon	simplification level
	 * @return			simplified building shape
	 */
	std::shared_ptr<Building> BuildingSimplification::simplifyBuildingByOpenCV(const cv::Size& size, std::shared_ptr<util::Layer> layer, float snap_vertex_threshold, float snap_edge_threshold, float epsilon) {
		std::vector<util::Polygon> polygons = util::findContours(layer->slices[0]);

		if (polygons.size() == 0) throw "No building voxel is found in this layer.";

		util::Polygon simplified_polygon = OpenCVSimplification::simplify(layer->selectRepresentativeSlice(), epsilon);
		cv::Mat_<float> mat = (cv::Mat_<float>(3, 3) << 1, 0, -size.width / 2, 0, -1, size.height / 2, 0, 0, 1);
		simplified_polygon.transform(mat);
		std::shared_ptr<Building> building = std::shared_ptr<Building>(new Building(simplified_polygon, layer->bottom_height, layer->top_height));

		for (int i = 0; i < layer->children.size(); i++) {
			try {
				std::shared_ptr<Building> child = simplifyBuildingByOpenCV(size, layer->children[i], snap_vertex_threshold, snap_edge_threshold, epsilon);
				util::snapPolygon(building->footprint.contour.points, child->footprint.contour.points, snap_vertex_threshold, snap_edge_threshold);
				OpenCVSimplification::decomposePolygon(child->footprint);
				building->children.push_back(child);
			}
			catch (...) {
			}
		}

		return building;
	}

	/**
	 * Simplify the shape of a building.
	 *
	 * @param layer			layer
	 * @param resolution	simplification level
	 * @return				simplified building shape
	 */
	std::shared_ptr<Building> BuildingSimplification::simplifyBuildingByOurCustom(const cv::Size& size, std::shared_ptr<util::Layer> layer, float snap_vertex_threshold, float snap_edge_threshold, int resolution, float angle, int dx, int dy) {
		std::vector<util::Polygon> polygons = util::findContours(layer->slices[0]);

		if (polygons.size() == 0) throw "No building voxel is found in this layer.";

		util::Polygon simplified_polygon = OurCustomSimplification::simplify(layer->selectRepresentativeSlice(), resolution, angle, dx, dy);
		cv::Mat_<float> mat = (cv::Mat_<float>(3, 3) << 1, 0, -size.width / 2, 0, -1, size.height / 2, 0, 0, 1);
		simplified_polygon.transform(mat);
		std::shared_ptr<Building> building = std::shared_ptr<Building>(new Building(simplified_polygon, layer->bottom_height, layer->top_height));

		for (int i = 0; i < layer->children.size(); i++) {
			try {
				std::shared_ptr<Building> child = simplifyBuildingByOurCustom(size, layer->children[i], snap_vertex_threshold, snap_edge_threshold, resolution, angle, dx, dy);
				util::snapPolygon(building->footprint.contour.points, child->footprint.contour.points, snap_vertex_threshold, snap_edge_threshold);
				OurCustomSimplification::decomposePolygon(child->footprint);
				building->children.push_back(child);
			}
			catch (...) {
			}
		}

		return building;
	}

	std::shared_ptr<Building> BuildingSimplification::simplifyBuildingByCurve(const cv::Size& size, std::shared_ptr<util::Layer> layer, float snap_vertex_threshold, float snap_edge_threshold, float epsilon, float curve_threshold) {
		// to be implemented

		throw "Not implemented";

		return NULL;
	}

	float BuildingSimplification::calculateCost(const util::Polygon& polygon, std::shared_ptr<util::Layer> layer) {
		// calculate average IOU
		cv::Size size(layer->slices[0].cols, layer->slices[0].rows);
		cv::Mat_<uchar> polygon_img;
		util::createImageFromPolygon(size.width, size.height, polygon, cv::Point(0, 0), polygon_img);

		float iou = 0;
		for (int i = 0; i < layer->slices.size(); i++) {
			iou += util::calculateIOU(layer->slices[i], polygon_img);
		}
		iou /= layer->slices.size();

		float error = 1 - iou;

		// number of primitive shapes
		int num_prim = polygon.primitive_shapes.size();

		return error + num_prim * 0.02;
	}
}