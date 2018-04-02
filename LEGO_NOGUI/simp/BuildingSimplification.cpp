#include "BuildingSimplification.h"
#include "DPSimplification.h"
#include "../util/ContourUtils.h"
#include "DPSimplification.h"
#include "RightAngleSimplification.h"
#include "CurveSimplification.h"
#include "CurveRightAngleSimplification.h"

namespace simp {

	std::vector<std::shared_ptr<util::BuildingLayer>> BuildingSimplification::simplifyBuildings(const std::vector<std::shared_ptr<util::BuildingLayer>>& raw_buildings, int algorithm, bool record_stats, float alpha, float layering_threshold, float epsilon, int resolution, float curve_threshold, float angle_threshold) {
		std::vector<std::shared_ptr<util::BuildingLayer>> buildings;

		std::vector<std::tuple<float, long long, int>> records;

		time_t start = clock();
		setbuf(stdout, NULL);
		printf("Processing building");
		for (int i = 0; i < raw_buildings.size(); i++) {
			printf("\rProcessing building %d  ", i + 1);

			try {
				
				std::shared_ptr<util::BuildingLayer> building;
				if (algorithm == ALG_ALL) {
					float angle = -1;
					int dx = -1;
					int dy = -1;
					building = simplifyBuildingByAll(i, util::DisjointVoxelData::layering(raw_buildings[i], layering_threshold), alpha, angle, dx, dy, records);
				}
				else if (algorithm == ALG_DP) {
					std::shared_ptr<util::BuildingLayer> layer = util::DisjointVoxelData::layering(raw_buildings[i], layering_threshold);
					building = simplifyBuildingByDP(i, layer, alpha, epsilon);
				}
				else if (algorithm == ALG_RIGHTANGLE) {
					float angle = -1;
					int dx = -1;
					int dy = -1;
					building = simplifyBuildingByRightAngle(i, util::DisjointVoxelData::layering(raw_buildings[i], layering_threshold), alpha, resolution, angle, dx, dy);
				}
				else if (algorithm == ALG_CURVE) {
					building = simplifyBuildingByCurve(i, util::DisjointVoxelData::layering(raw_buildings[i], layering_threshold), alpha, epsilon, curve_threshold);
				}
				else if (algorithm == ALG_CURVE_RIGHTANGLE) {
					building = simplifyBuildingByCurveRightAngle(i, util::DisjointVoxelData::layering(raw_buildings[i], layering_threshold), alpha, epsilon, curve_threshold, angle_threshold);
				}

				buildings.push_back(building);
			}
			catch (...) {}
		}
		printf("\n");
		time_t end = clock();
		std::cout << "Time elapsed " << (double)(end - start) / CLOCKS_PER_SEC << " sec." << std::endl;

		if (record_stats) {
			std::ofstream out("records.txt");
			for (int i = 0; i < records.size(); i++) {
				float error = std::get<0>(records[i]);
				long long num_primitive_shapes = std::get<1>(records[i]);
				int selected_algorithm = std::get<2>(records[i]);
				out << error << " " << num_primitive_shapes << " " << selected_algorithm << std::endl;
			}
			out.close();
		}

		return buildings;
	}

	std::shared_ptr<util::BuildingLayer> BuildingSimplification::simplifyBuildingByAll(int building_id, std::shared_ptr<util::BuildingLayer> layer, float alpha, float angle, int dx, int dy, std::vector<std::tuple<float, long long, int>>& records) {
		std::vector<util::Polygon> contours = layer->selectRepresentativeContours();
		
		// get baseline cost
		std::vector<util::Polygon> baseline_polygons;
		std::vector<float> baseline_costs(3, 0);
		for (int i = 0; i < contours.size(); i++) {
			baseline_polygons.push_back(DPSimplification::simplify(contours[i], 0.5));
			std::vector<float> costs = calculateCost(baseline_polygons[i], contours[i], layer->top_height - layer->bottom_height);
			for (int j = 0; j < 3; j++) {
				baseline_costs[j] += costs[j];
			}
		}
		
		std::vector<util::Polygon> best_simplified_polygons;
		for (int i = 0; i < contours.size(); i++) {
			util::Polygon best_simplified_polygon;

			float best_cost = std::numeric_limits<float>::max();
			int best_algorithm = ALG_UNKNOWN;
			float best_error = 0.0f;
			int best_num_primitive_shapes = 0;

			// try Douglas-Peucker
			try {
				float epsilon;
				if (alpha == 0.0) epsilon = 18;
				else if (alpha < 0.2) epsilon = 12;
				else if (alpha < 0.4) epsilon = 10;
				else if (alpha < 0.6) epsilon = 8;
				else if (alpha < 0.8) epsilon = 4;
				else epsilon = 4;

				util::Polygon simplified_polygon = DPSimplification::simplify(contours[i], epsilon);
				std::vector<float> costs = calculateCost(simplified_polygon, contours[i], layer->top_height - layer->bottom_height);
				float cost = alpha * costs[0] / costs[1] + (1 - alpha) * costs[2] / baseline_costs[2];

				if (cost < best_cost) {
					best_algorithm = ALG_DP;
					best_cost = cost;
					best_simplified_polygon = simplified_polygon;

					best_error = costs[0] / costs[1];
					best_num_primitive_shapes = costs[2];
				}
			}
			catch (...) {}

			// try right angle
			try {
				int resolution;
				if (alpha == 0.0) resolution = 18;
				else if (alpha < 0.2) resolution = 12;
				else if (alpha < 0.4) resolution = 10;
				else if (alpha < 0.6) resolution = 8;
				else if (alpha < 0.8) resolution = 4;
				else resolution = 4;

				util::Polygon simplified_polygon = RightAngleSimplification::simplify(contours[i], resolution, angle, dx, dy);
				std::vector<float> costs = calculateCost(simplified_polygon, contours[i], layer->top_height - layer->bottom_height);
				float cost = alpha * costs[0] / costs[1] + (1 - alpha) * costs[2] / baseline_costs[2];
				if (cost < best_cost) {
					best_algorithm = ALG_RIGHTANGLE;
					best_cost = cost;
					best_simplified_polygon = simplified_polygon;

					best_error = costs[0] / costs[1];
					best_num_primitive_shapes = costs[2];
				}
			}
			catch (...) {}

			// try curve
			try {
				float epsilon;
				if (alpha == 0.0) epsilon = 18;
				else if (alpha < 0.2) epsilon = 12;
				else if (alpha < 0.4) epsilon = 10;
				else if (alpha < 0.6) epsilon = 8;
				else if (alpha < 0.8) epsilon = 4;
				else epsilon = 4;

				float curve_threshold;
				if (alpha < 0.2) curve_threshold = 1.5f;
				else curve_threshold = 1.0f;

				util::Polygon simplified_polygon = CurveSimplification::simplify(contours[i], epsilon, curve_threshold);
				std::vector<float> costs = calculateCost(simplified_polygon, contours[i], layer->top_height - layer->bottom_height);
				float cost = alpha * costs[0] / costs[1] + (1 - alpha) * costs[2] / baseline_costs[2];

				if (cost < best_cost) {
					best_algorithm = ALG_CURVE;
					best_cost = cost;
					best_simplified_polygon = simplified_polygon;

					best_error = costs[0] / costs[1];
					best_num_primitive_shapes = costs[2];
				}
			}
			catch (...) {}

			// try curve + right angle
			try {
				float epsilon;
				if (alpha == 0.0) epsilon = 18;
				else if (alpha < 0.2) epsilon = 162;
				else if (alpha < 0.4) epsilon = 10;
				else if (alpha < 0.6) epsilon = 8;
				else if (alpha < 0.8) epsilon = 4;
				else epsilon = 4;

				float curve_threshold;
				if (alpha < 0.2) curve_threshold = 1.5f;
				else curve_threshold = 1.0f;

				float angle_threshold = 10.0f / 180.0f * CV_PI;

				util::Polygon simplified_polygon = CurveRightAngleSimplification::simplify(contours[i], epsilon, curve_threshold, angle_threshold);
				std::vector<float> costs = calculateCost(simplified_polygon, contours[i], layer->top_height - layer->bottom_height);
				float cost = alpha * costs[0] / costs[1] + (1 - alpha) * costs[2] / baseline_costs[2];

				if (cost < best_cost) {
					best_algorithm = ALG_CURVE;
					best_cost = cost;
					best_simplified_polygon = simplified_polygon;

					best_error = costs[0] / costs[1];
					best_num_primitive_shapes = costs[2];
				}
			}
			catch (...) {}
			
			if (best_algorithm == ALG_UNKNOWN) continue;

			best_simplified_polygons.push_back(best_simplified_polygon);
			records.push_back(std::make_tuple(best_error, best_num_primitive_shapes, best_algorithm));
		}
		
		std::shared_ptr<util::BuildingLayer> building = std::shared_ptr<util::BuildingLayer>(new util::BuildingLayer(building_id, best_simplified_polygons, layer->bottom_height, layer->top_height));

		if (layer->child) {
			try {
				std::shared_ptr<util::BuildingLayer> child = simplifyBuildingByAll(building_id, layer->child, alpha, -1, -1, -1, records);
				building->child = child;
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
	std::shared_ptr<util::BuildingLayer> BuildingSimplification::simplifyBuildingByDP(int building_id, std::shared_ptr<util::BuildingLayer> layer, float alpha, float epsilon) {
		std::vector<util::Polygon> contours = layer->selectRepresentativeContours();

		// simplify the polygon
		std::vector<util::Polygon> simplified_polygons;
		for (int i = 0; i < contours.size(); i++) {
			try {
				util::Polygon simplified_polygon = DPSimplification::simplify(contours[i], epsilon);
				float polygon_area = util::calculateArea(simplified_polygon);
				
				simplified_polygons.push_back(simplified_polygon);
			}
			catch (...) {}
		}
		if (simplified_polygons.size() == 0) throw "Simplification failed.";

		std::shared_ptr<util::BuildingLayer> building = std::shared_ptr<util::BuildingLayer>(new util::BuildingLayer(building_id, simplified_polygons, layer->bottom_height, layer->top_height));

		if (layer->child) {
			try {
				std::shared_ptr<util::BuildingLayer> child = simplifyBuildingByDP(building_id, layer->child, alpha, epsilon);
				building->child = child;
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
	std::shared_ptr<util::BuildingLayer> BuildingSimplification::simplifyBuildingByRightAngle(int building_id, std::shared_ptr<util::BuildingLayer> layer, float alpha, int resolution, float angle, int dx, int dy) {
		std::vector<util::Polygon> contours = layer->selectRepresentativeContours();
		std::vector<util::Polygon> simplified_polygons;
		for (int i = 0; i < contours.size(); i++) {
			try {
				simplified_polygons.push_back(RightAngleSimplification::simplify(contours[i], resolution, angle, dx, dy));
			}
			catch (...) {}
		}
		if (simplified_polygons.size() == 0) throw "Simplification failed.";

		std::shared_ptr<util::BuildingLayer> building = std::shared_ptr<util::BuildingLayer>(new util::BuildingLayer(building_id, simplified_polygons, layer->bottom_height, layer->top_height));

		if (layer->child) {
			try {
				std::shared_ptr<util::BuildingLayer> child = simplifyBuildingByRightAngle(building_id, layer->child, alpha, resolution, angle, dx, dy);
				building->child = child;
			}
			catch (...) {
			}
		}

		return building;
	}

	std::shared_ptr<util::BuildingLayer> BuildingSimplification::simplifyBuildingByCurve(int building_id, std::shared_ptr<util::BuildingLayer> layer, float alpha, float epsilon, float curve_threshold) {
		std::vector<util::Polygon> contours = layer->selectRepresentativeContours();
		std::vector<util::Polygon> simplified_polygons;
		for (int i = 0; i < contours.size(); i++) {
			try {
				simplified_polygons.push_back(CurveSimplification::simplify(contours[i], epsilon, curve_threshold));
			}
			catch (...) {}
		}
		if (simplified_polygons.size() == 0) throw "Simplification failed.";

		std::shared_ptr<util::BuildingLayer> building = std::shared_ptr<util::BuildingLayer>(new util::BuildingLayer(building_id, simplified_polygons, layer->bottom_height, layer->top_height));

		if (layer->child) {
			try {
				std::shared_ptr<util::BuildingLayer> child = simplifyBuildingByCurve(building_id, layer->child, alpha, epsilon, curve_threshold);
				building->child = child;
			}
			catch (...) {
			}
		}

		return building;
	}

	std::shared_ptr<util::BuildingLayer> BuildingSimplification::simplifyBuildingByCurveRightAngle(int building_id, std::shared_ptr<util::BuildingLayer> layer, float alpha, float epsilon, float curve_threshold, float angle_threshold) {
		std::vector<util::Polygon> contours = layer->selectRepresentativeContours();
		std::vector<util::Polygon> simplified_polygons;
		for (int i = 0; i < contours.size(); i++) {
			try {
				simplified_polygons.push_back(CurveRightAngleSimplification::simplify(contours[i], epsilon, curve_threshold, angle_threshold));
			}
			catch (...) {}
		}
		if (simplified_polygons.size() == 0) throw "Simplification failed.";

		std::shared_ptr<util::BuildingLayer> building = std::shared_ptr<util::BuildingLayer>(new util::BuildingLayer(building_id, simplified_polygons, layer->bottom_height, layer->top_height));

		if (layer->child) {
			try {
				std::shared_ptr<util::BuildingLayer> child = simplifyBuildingByCurveRightAngle(building_id, layer->child, alpha, epsilon, curve_threshold, angle_threshold);
				building->child = child;
			}
			catch (...) {
			}
		}

		return building;
	}

	/**
	 * Calculate cost for the layer/
	 *
	 * @param size					XY dimension of the voxel data
	 * @param simplified_polygon	the simplified polygon for which we calculate the cost
	 * @param layer					layer for which we calculate the cost
	 * @param alpha					weight ratio of the error term to the simplicity term in the cost function
	 * @return						three values, (1-IOU) * area, area, and #primitive shapes
	 */
	std::vector<float> BuildingSimplification::calculateCost(const util::Polygon& simplified_polygon, const util::Polygon& polygon, int height) {
		std::vector<float> ans(3, 0);

		// calculate IOU
		float slice_area = util::calculateArea(polygon);
		float iou = util::calculateIOU(simplified_polygon, polygon);
		ans[0] = (1 - iou) * slice_area * height * 0.5;
		ans[1] = slice_area * height;
		
		ans[2] = simplified_polygon.primitive_shapes.size();

		return ans;
	}

}