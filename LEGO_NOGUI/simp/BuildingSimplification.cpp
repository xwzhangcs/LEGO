#include "BuildingSimplification.h"
#include "DPSimplification.h"
#include "../util/ContourUtils.h"
#include "DPSimplification.h"
#include "RightAngleSimplification.h"
#include "CurveSimplification.h"
#include "CurveRightAngleSimplification.h"
#include "EfficientRansacSimplification.h"
#include "../util/EfficientRansacCurveDetector.h"

namespace simp {

	/**
	 * Simplify all the buildings.
	 *
	 * @param voxel_buildings			Input buildings, each of which is represented by a stack of contour polygons.
	 * @param algorithms				Specified simplification algorithms with their parameter values
	 * @param record_stats				If this is true, the statistics will be recorded.
	 * @param min_num_slices_per_layer	If the number of the slices in the layer is less than this threshold, the layer will be merged to its lower layer or will be removed if it is the top layer.
	 * @param alpha						The weight balance between simplicity and accuracy (0 - simple vs 1 - accurate)
	 * @param layering_threshold		Layering threshold
	 * @param snapping_threshold		Snapping threshold
	 * @param orientation				Principle orientation of the contour in radian
	 * @param min_contour_area			Minimum area of the contour [pixel^2]. Note: the unit is already converted from m^2 to pixel^2.
	 * @param allow_triangle_contour	True if a triangle is allowed as a simplified contour shape
	 * @param min_hole_ratio			The minimum area ratio of a hole to the contour. If the area of the hole is too small, it will be removed.
	 */
	std::vector<std::shared_ptr<util::BuildingLayer>> BuildingSimplification::simplifyBuildings(std::vector<util::VoxelBuilding>& voxel_buildings, std::map<int, std::vector<double>>& algorithms, bool record_stats, int min_num_slices_per_layer, float alpha, float layering_threshold, float snapping_threshold, float orientation, float min_contour_area, float max_obb_ratio, bool allow_triangle_contour, bool allow_overhang, float min_hole_ratio, const std::vector<regularizer::Config>& regularizer_configs) {
		std::vector<std::shared_ptr<util::BuildingLayer>> buildings;

		std::vector<std::tuple<float, long long, int>> records;

		time_t start = clock();
		setbuf(stdout, NULL);
		for (int i = 0; i < voxel_buildings.size(); i++) {
			std::vector<std::shared_ptr<util::BuildingLayer>> components = util::DisjointVoxelData::layering(voxel_buildings[i], layering_threshold, min_num_slices_per_layer);
			for (auto component : components) {
				try {
					// Better approach using efficient RANSAC
					int height = component->getTopHeight();
					bool curve_preferred = height < 121 && (component->top_height - component->bottom_height < 37) && component->top_height < 53 && util::EfficientRansacCurveDetector::detect2(component->raw_footprints);

					/*
					int height = component->getTopHeight();
					bool curve_preferred = height < 121 && (component->top_height - component->bottom_height < 37) && component->top_height < 53 && util::EfficientRansacCurveDetector::detect(contours[0]);
					*/

					std::shared_ptr<util::BuildingLayer> building;
					building = simplifyBuildingByAll(i, component, {}, algorithms, alpha, snapping_threshold, orientation, min_contour_area, max_obb_ratio, allow_triangle_contour, allow_overhang, min_hole_ratio, curve_preferred, records);

					// regularizer
					if (regularizer_configs.size() >= 0)
					{
						
						std::vector<std::shared_ptr<util::BuildingLayer>>layers;
						std::vector<std::pair<int, int>>layers_relationship;
						generateVectorForAllLayers(building, 0, layers, layers_relationship);
						std::cout << "layers_relationship.size() is " << layers_relationship.size() << std::endl;
						// test layer_relationship
						for (int l = 0; l < layers_relationship.size(); l++)
							std::cout <<"(" <<layers_relationship[l].first << ", " << layers_relationship[l].second <<")"<< std::endl;
					}

					buildings.push_back(building);
				}
				catch (...) {}
			}
		}
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

	/**
	 * This is just for the old interface. This will be depricated in the future.
	 */
	std::vector<std::shared_ptr<util::BuildingLayer>> BuildingSimplification::simplifyBuildings(std::vector<util::VoxelBuilding>& voxel_buildings, int algorithm, bool record_stats, int min_num_slices_per_layer, float alpha, float layering_threshold, float epsilon, int resolution, float curve_threshold, float angle_threshold, float min_hole_ratio) {
		std::vector<std::shared_ptr<util::BuildingLayer>> buildings;

		std::map<int, std::vector<double>> algorithms;
		algorithms[simp::BuildingSimplification::ALG_DP] = { epsilon };
		algorithms[simp::BuildingSimplification::ALG_RIGHTANGLE] = { (double)resolution, 1.0 };
		algorithms[simp::BuildingSimplification::ALG_CURVE] = { epsilon, curve_threshold };
		algorithms[simp::BuildingSimplification::ALG_CURVE_RIGHTANGLE] = { epsilon, curve_threshold, angle_threshold };

		return simplifyBuildings(voxel_buildings, algorithms, record_stats, min_num_slices_per_layer, alpha, layering_threshold, 0, 0, 0, 10, true, true, min_hole_ratio);
	}
	
	/**
	 * Simplify the building by using all the simplification methods that are specified in "algorithms".
	 *
	 * @param building_id			Building id
	 * @param layer					The root layer of the building, which is the bottom layer.
	 * @param algorithms			The set of algorithms with their parameter values
	 * @param alpha					The weight balance between simplicity and accuracy ( 0 - simple vs 1 - accurate)
	 * @param snapping_threshold	The maximum distance for snapping
	 * @param orientation			The principle orientation of the building in radian
	 * @param min_hole_ratio		The minimum area ratio of the hole to the contour
	 * @param records				The statistics will be recorded.
	 */
	std::shared_ptr<util::BuildingLayer> BuildingSimplification::simplifyBuildingByAll(int building_id, std::shared_ptr<util::BuildingLayer> layer, const std::vector<util::Polygon>& parent_contours, std::map<int, std::vector<double>>& algorithms, float alpha, float snapping_threshold, float orientation, float min_contour_area, float max_obb_ratio, bool allow_triangle_contour, bool allow_overhang, float min_hole_ratio, bool curve_preferred, std::vector<std::tuple<float, long long, int>>& records) {
		std::vector<util::Polygon> contours = layer->selectRepresentativeContours();

		// get baseline cost
		std::vector<util::Polygon> baseline_polygons;
		std::vector<float> baseline_costs(3, 0);
		for (int i = 0; i < contours.size(); i++) {
			baseline_polygons.push_back(DPSimplification::simplify(contours[i], 0.5, min_hole_ratio));
			std::vector<float> costs = calculateCost(baseline_polygons[i], contours[i], layer->top_height - layer->bottom_height);
			for (int j = 0; j < 3; j++) {
				baseline_costs[j] += costs[j];
			}
		}

		std::vector<util::Polygon> best_simplified_polygons;
		bool right_angle_for_all_contours = true;
		for (int i = 0; i < contours.size(); i++) {
			util::Polygon best_simplified_polygon;

			float best_cost = std::numeric_limits<float>::max();
			int best_algorithm = ALG_UNKNOWN;
			float best_error = 0.0f;
			int best_num_primitive_shapes = 0;

			// try Douglas-Peucker
			if (algorithms.find(ALG_DP) != algorithms.end()) {
				try {
					double epsilon = algorithms[ALG_DP][0];
					util::Polygon simplified_polygon = DPSimplification::simplify(contours[i], epsilon, min_hole_ratio);
					if (!util::isSimple(simplified_polygon.contour)) throw "Contour is self-intersecting.";

					// check if the shape is a triangle
					if (!allow_triangle_contour && simplified_polygon.contour.size() <= 3) throw "Triangle is not allowed.";

					// check the OBB ratio
					cv::Mat_<float> m;
					cv::Rect obb = util::calculateOBB(simplified_polygon.contour.points, m);
					if (obb.width == 0 || obb.height == 0 || obb.width / obb.height > max_obb_ratio || obb.height / obb.width > max_obb_ratio) throw "OBB ratio is exceeded the threshold.";

					// calculate cost
					std::vector<float> costs = calculateCost(simplified_polygon, contours[i], layer->top_height - layer->bottom_height);
					float cost = alpha * costs[0] / costs[1] + (1 - alpha) * costs[2] / baseline_costs[2];
					if (cost < best_cost) {
						best_algorithm = ALG_DP;
						right_angle_for_all_contours = false;
						best_cost = cost;
						best_simplified_polygon = simplified_polygon;

						best_error = costs[0] / costs[1];
						best_num_primitive_shapes = costs[2];
					}
				}
				catch (...) {}
			}

			// try right angle
			if (algorithms.find(ALG_RIGHTANGLE) != algorithms.end()) {
				try {
					int resolution = algorithms[ALG_RIGHTANGLE][0];
					bool optimization = algorithms[ALG_RIGHTANGLE][1] > 0.0;
					util::Polygon simplified_polygon = RightAngleSimplification::simplify(contours[i], resolution, orientation, min_hole_ratio, optimization);
					if (!util::isSimple(simplified_polygon.contour)) throw "Contour is self-intersecting.";

					// check if the shape is a triangle
					if (!allow_triangle_contour && simplified_polygon.contour.size() <= 3) throw "Triangle is not allowed.";

					// check the OBB ratio
					cv::Mat_<float> m;
					cv::Rect obb = util::calculateOBB(simplified_polygon.contour.points, m);
					if (obb.width == 0 || obb.height == 0 || obb.width / obb.height > max_obb_ratio || obb.height / obb.width > max_obb_ratio) throw "OBB ratio is exceeded the threshold.";

					// calculate cost
					std::vector<float> costs = calculateCost(simplified_polygon, contours[i], layer->top_height - layer->bottom_height);
					float cost = alpha * costs[0] / costs[1] + (1 - alpha) * costs[2] / baseline_costs[2];
					if (curve_preferred) cost *= 2;
					else cost *= 0.5;
					if (cost < best_cost) {
						best_algorithm = ALG_RIGHTANGLE;
						best_cost = cost;
						best_simplified_polygon = simplified_polygon;

						best_error = costs[0] / costs[1];
						best_num_primitive_shapes = costs[2];
					}
				}
				catch (...) {}
			}

			// try curve
			if (algorithms.find(ALG_CURVE) != algorithms.end()) {
				try {
					float epsilon = algorithms[ALG_CURVE][0];
					float curve_threshold = algorithms[ALG_CURVE][1];
					util::Polygon simplified_polygon = CurveSimplification::simplify(contours[i], epsilon, curve_threshold, orientation, min_hole_ratio);
					if (!util::isSimple(simplified_polygon.contour)) throw "Contour is self-intersecting.";

					// check if the shape is a triangle
					if (!allow_triangle_contour && simplified_polygon.contour.size() <= 3) throw "Triangle is not allowed.";

					// check the OBB ratio
					cv::Mat_<float> m;
					cv::Rect obb = util::calculateOBB(simplified_polygon.contour.points, m);
					if (obb.width == 0 || obb.height == 0 || obb.width / obb.height > max_obb_ratio || obb.height / obb.width > max_obb_ratio) throw "OBB ratio is exceeded the threshold.";
					
					// calculate cost
					std::vector<float> costs = calculateCost(simplified_polygon, contours[i], layer->top_height - layer->bottom_height);
					float cost = alpha * costs[0] / costs[1] + (1 - alpha) * costs[2] / baseline_costs[2];
					if (cost < best_cost) {
						best_algorithm = ALG_CURVE;
						right_angle_for_all_contours = false;
						best_cost = cost;
						best_simplified_polygon = simplified_polygon;

						best_error = costs[0] / costs[1];
						best_num_primitive_shapes = costs[2];
					}
				}
				catch (...) {}
			}

			// try curve + right angle
			if (algorithms.find(ALG_CURVE_RIGHTANGLE) != algorithms.end()) {
				try {
					float epsilon = algorithms[ALG_CURVE_RIGHTANGLE][0];
					float curve_threshold = algorithms[ALG_CURVE_RIGHTANGLE][1];
					float angle_threshold = algorithms[ALG_CURVE_RIGHTANGLE][2];
					util::Polygon simplified_polygon = CurveRightAngleSimplification::simplify(contours[i], epsilon, curve_threshold, angle_threshold, orientation, min_hole_ratio);
					if (!util::isSimple(simplified_polygon.contour)) throw "Contour is self-intersecting.";
					
					// check if the shape is a triangle
					if (!allow_triangle_contour && simplified_polygon.contour.size() <= 3) throw "Triangle is not allowed.";

					// check the OBB ratio
					cv::Mat_<float> m;
					cv::Rect obb = util::calculateOBB(simplified_polygon.contour.points, m);
					if (obb.width == 0 || obb.height == 0 || obb.width / obb.height > max_obb_ratio || obb.height / obb.width > max_obb_ratio) throw "OBB ratio is exceeded the threshold.";

					// calculate cost
					std::vector<float> costs = calculateCost(simplified_polygon, contours[i], layer->top_height - layer->bottom_height);
					float cost = alpha * costs[0] / costs[1] + (1 - alpha) * costs[2] / baseline_costs[2];
					if (curve_preferred) cost *= 0.5;
					if (cost < best_cost) {
						best_algorithm = ALG_CURVE_RIGHTANGLE;
						right_angle_for_all_contours = false;
						best_cost = cost;
						best_simplified_polygon = simplified_polygon;

						best_error = costs[0] / costs[1];
						best_num_primitive_shapes = costs[2];
					}
				}
				catch (...) {}
			}

			// try efficient ransac
			if (algorithms.find(ALG_EFFICIENT_RANSAC) != algorithms.end()) {
				try {
					util::Polygon simplified_polygon = EfficientRansacSimplification::simplify(contours[i], algorithms[ALG_EFFICIENT_RANSAC], orientation, min_hole_ratio);
					if (!util::isSimple(simplified_polygon.contour)) throw "Contour is self-intersecting.";

					// check if the shape is a triangle
					if (!allow_triangle_contour && simplified_polygon.contour.size() <= 3) throw "Triangle is not allowed.";

					// check the OBB ratio
					cv::Mat_<float> m;
					cv::Rect obb = util::calculateOBB(simplified_polygon.contour.points, m);
					if (obb.width == 0 || obb.height == 0 || obb.width / obb.height > max_obb_ratio || obb.height / obb.width > max_obb_ratio) throw "OBB ratio is exceeded the threshold.";

					// calculate cost
					std::vector<float> costs = calculateCost(simplified_polygon, contours[i], layer->top_height - layer->bottom_height);
					float cost = alpha * costs[0] / costs[1] + (1 - alpha) * costs[2] / baseline_costs[2];
					if (curve_preferred) cost *= 0.5;
					if (cost < best_cost) {
						best_algorithm = ALG_EFFICIENT_RANSAC;
						right_angle_for_all_contours = false;
						best_cost = cost;
						best_simplified_polygon = simplified_polygon;

						best_error = costs[0] / costs[1];
						best_num_primitive_shapes = costs[2];
					}
				}
				catch (...) { std::cout << "exception" << std::endl; }
			}

			if (best_algorithm == ALG_UNKNOWN) {
				// try Douglas-Peucker when no method works
				try {
					float epsilon = 2;
					util::Polygon simplified_polygon = DPSimplification::simplify(contours[i], epsilon, min_hole_ratio);
					if (!util::isSimple(simplified_polygon.contour)) throw "Contour is self-intersecting.";
					if (!allow_triangle_contour && simplified_polygon.contour.size() <= 3) throw "Triangle is not allowed.";
					std::vector<float> costs = calculateCost(simplified_polygon, contours[i], layer->top_height - layer->bottom_height);
					float cost = alpha * costs[0] / costs[1] + (1 - alpha) * costs[2] / baseline_costs[2];

					if (cost < best_cost) {
						best_algorithm = ALG_DP;
						right_angle_for_all_contours = false;
						best_cost = cost;
						best_simplified_polygon = simplified_polygon;

						best_error = costs[0] / costs[1];
						best_num_primitive_shapes = costs[2];
					}
				}
				catch (...) {}
			}

			if (best_algorithm == ALG_UNKNOWN) continue;

			if (best_algorithm == ALG_RIGHTANGLE) {
				std::cout << "Selected algorithm: RA" << std::endl;
			}
			else if (best_algorithm == ALG_CURVE || best_algorithm == ALG_CURVE_RIGHTANGLE) {
				std::cout << "Selected algorithm: CSRA" << std::endl;
			}
			else if (best_algorithm == ALG_EFFICIENT_RANSAC) {
				std::cout << "Selected algorithm: EFFICIENT RANSAC" << std::endl;
			}
			else {
				std::cout << "Selected algorithm: DP" << std::endl;
			}

			// snap the edges
			/*if (parent_contours.size() > 0 && snapping_threshold > 0) {
				if (best_algorithm == ALG_RIGHTANGLE) {
					util::snapPolygon(parent_contours, best_simplified_polygon, snapping_threshold);
				}
				else {
					util::snapPolygon2(parent_contours, best_simplified_polygon, snapping_threshold);
				}
			}*/

			// crop the contour such that it is completely inside the parent contours,
			// and add the cropped contours to the results.
			if (/*parent_contours.size() > 0 && !allow_overhang*/false) {
				try {
					for (int j = 0; j < parent_contours.size(); j++) {
						std::vector<util::Polygon> cropped_simplified_polygons = util::intersection(best_simplified_polygon, parent_contours[j]);
						for (int k = 0; k < cropped_simplified_polygons.size(); k++) {
							if (isSimple(cropped_simplified_polygons[k])) {
								best_simplified_polygons.push_back(cropped_simplified_polygons[k]);
							}
						}
					}
				} 
				catch (...) {
					if (isSimple(best_simplified_polygon)) {
						best_simplified_polygons.push_back(best_simplified_polygon);
					}
				}
			}
			else {
				best_simplified_polygons.push_back(best_simplified_polygon);
			}

			records.push_back(std::make_tuple(best_error, best_num_primitive_shapes, best_algorithm));
		}


		// remove too small contours
		if (parent_contours.size() > 0) {
			for (int i = best_simplified_polygons.size() - 1; i >= 0; i--) {
				if (calculateArea(best_simplified_polygons[i]) < min_contour_area) {
					best_simplified_polygons.erase(best_simplified_polygons.begin() + i);
				}
			}
		}

		if (best_simplified_polygons.size() == 0) throw "No valid contour.";

		std::shared_ptr<util::BuildingLayer> building = std::shared_ptr<util::BuildingLayer>(new util::BuildingLayer(building_id, best_simplified_polygons, layer->bottom_height, layer->top_height));

		for (auto child_layer : layer->children) {
			try {
				float contour_area = 0;
				for (int i = 0; i < contours.size(); i++) {
					contour_area += util::calculateArea(contours[i]);
				}

				float next_contour_area = 0;
				for (int i = 0; i < child_layer->raw_footprints[0].size(); i++) {
					next_contour_area += util::calculateArea(child_layer->raw_footprints[0][i]);
				}

				std::shared_ptr<util::BuildingLayer> child = simplifyBuildingByAll(building_id, child_layer, best_simplified_polygons, algorithms, alpha, snapping_threshold, orientation, min_contour_area, max_obb_ratio, allow_triangle_contour, allow_overhang, min_hole_ratio, curve_preferred, records);
				building->children.push_back(child);
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
		ans[0] = (1 - iou) * slice_area * height;
		ans[1] = slice_area * height;
		
		// calculate #primitive shapes
		ans[2] = simplified_polygon.contour.size();

		return ans;
	}

	/**
	* Generate images for all layers/
	*
	* @param		input root building
	* @param		config files
	*/
	std::shared_ptr<util::BuildingLayer> BuildingSimplification::regularizerBuilding(std::shared_ptr<util::BuildingLayer> building, std::vector<regularizer::Config>& regularizer_configs){
		return building;
	}

	/**
	* Generate images for all layers/
	*
	* @param		root
	* @param		image index
	*/
	int BuildingSimplification::generateVectorForAllLayers(std::shared_ptr<util::BuildingLayer> root, int layer_id, std::vector<std::shared_ptr<util::BuildingLayer>> & layers, std::vector<std::pair<int, int>>& layers_relationship){
		std::shared_ptr<util::BuildingLayer> building = std::shared_ptr<util::BuildingLayer>(new util::BuildingLayer(root->building_id, root->footprints, root->bottom_height, root->top_height));
		layers.push_back(building);
		int current_layer_id = layer_id;
		for (auto child_layer : root->children){
			layer_id++;
			layers_relationship.push_back(std::make_pair(current_layer_id, layer_id));
			layer_id = generateVectorForAllLayers(child_layer, layer_id, layers, layers_relationship);
		}
		return layer_id;
	}
}
