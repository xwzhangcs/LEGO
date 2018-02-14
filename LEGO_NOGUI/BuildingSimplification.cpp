#include "BuildingSimplification.h"
#include "OpenCVSimplification.h"
#include "ContourUtils.h"
#include "LayerVoxelData.h"
#include "OpenCVSimplification.h"
#include "OurCustomSimplification.h"

namespace simp {

	std::vector<std::shared_ptr<Building>> BuildingSimplification::simplifyBuildings(const util::DisjointVoxelData& disjointed_voxel_data, int algorithm, float alpha, float layering_threshold, float epsilon, int resolution, float curve_threshold) {
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
					building = simplifyBuildingByAll(size, layer, alpha);
				}
				else if (algorithm == ALG_OPENCV) {
					building = simplifyBuildingByOpenCV(size, layer, alpha, epsilon);
				}
				else if (algorithm == ALG_RIGHTANGLE) {
					float angle = -1;
					int dx = -1;
					int dy = -1;
					building = simplifyBuildingByOurCustom(size, layer, alpha, resolution, angle, dx, dy);
				}
				else if (algorithm == ALG_CURVE) {
					building = simplifyBuildingByCurve(size, layer, alpha, epsilon, curve_threshold);
				}

				buildings.push_back(building);
			}
			catch (...) {}
		}

		return buildings;
	}

	std::vector<float> BuildingSimplification::sumCost(const std::vector<std::shared_ptr<Building>>& buildings) {
		std::vector<float> costs(3, 0);

		for (int i = 0; i < buildings.size(); i++) {
			std::vector<float> c = sumCost(buildings[i]->children);
			
			for (int j = 0; j < c.size(); j++) {
				costs[j] += buildings[i]->costs[j] + c[j];
			}
		}

		return costs;
	}

	std::shared_ptr<Building> BuildingSimplification::simplifyBuildingByAll(const cv::Size& size, std::shared_ptr<util::Layer> layer, float alpha) {
		return NULL;
		/*
		std::vector<util::Polygon> polygons = util::findContours(layer->slices[0]);

		if (polygons.size() == 0) throw "No building voxel is found in this layer.";

		float best_cost = std::numeric_limits<float>::max();
		util::Polygon best_simplified_polygon;
		bool found_best = false;

		// try OpenCV
		try {
			util::Polygon simplified_polygon = OpenCVSimplification::simplify(layer->selectRepresentativeSlice(), 1);
			float cost = calculateCost(size, simplified_polygon, layer, alpha);
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
			float cost = calculateCost(size, simplified_polygon, layer, alpha);
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
				std::shared_ptr<Building> child = simplifyBuildingByAll(size, layer->children[i], alpha);
				building->children.push_back(child);
			}
			catch (...) {
			}
		}

		return building;
		*/
	}

	/**
	 * Simplify the shape of a building.
	 *
	 * @param layer		layer
	 * @param epsilon	simplification level
	 * @return			simplified building shape
	 */
	std::shared_ptr<Building> BuildingSimplification::simplifyBuildingByOpenCV(const cv::Size& size, std::shared_ptr<util::Layer> layer, float alpha, float epsilon) {
		std::vector<util::Polygon> polygons = util::findContours(layer->slices[0]);

		if (polygons.size() == 0) throw "No building voxel is found in this layer.";

		util::Polygon simplified_polygon = OpenCVSimplification::simplify(layer->selectRepresentativeSlice(), epsilon);

		// calculate cost
		std::vector<float> costs = calculateCost(size, simplified_polygon, layer, alpha);

		cv::Mat_<float> mat = (cv::Mat_<float>(3, 3) << 1, 0, -size.width / 2, 0, -1, size.height / 2, 0, 0, 1);
		simplified_polygon.transform(mat);
		std::shared_ptr<Building> building = std::shared_ptr<Building>(new Building(simplified_polygon, layer->bottom_height, layer->top_height));
		building->costs = costs;

		for (int i = 0; i < layer->children.size(); i++) {
			try {
				std::shared_ptr<Building> child = simplifyBuildingByOpenCV(size, layer->children[i], alpha, epsilon);
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
	std::shared_ptr<Building> BuildingSimplification::simplifyBuildingByOurCustom(const cv::Size& size, std::shared_ptr<util::Layer> layer, float alpha, int resolution, float angle, int dx, int dy) {
		std::vector<util::Polygon> polygons = util::findContours(layer->slices[0]);

		if (polygons.size() == 0) throw "No building voxel is found in this layer.";

		util::Polygon simplified_polygon = OurCustomSimplification::simplify(layer->selectRepresentativeSlice(), resolution, angle, dx, dy);

		// calculate cost
		std::vector<float> costs = calculateCost(size, simplified_polygon, layer, alpha);

		cv::Mat_<float> mat = (cv::Mat_<float>(3, 3) << 1, 0, -size.width / 2, 0, -1, size.height / 2, 0, 0, 1);
		simplified_polygon.transform(mat);
		std::shared_ptr<Building> building = std::shared_ptr<Building>(new Building(simplified_polygon, layer->bottom_height, layer->top_height));
		building->costs = costs;

		for (int i = 0; i < layer->children.size(); i++) {
			try {
				std::shared_ptr<Building> child = simplifyBuildingByOurCustom(size, layer->children[i], alpha, resolution, angle, dx, dy);
				OurCustomSimplification::decomposePolygon(child->footprint);
				building->children.push_back(child);
			}
			catch (...) {
			}
		}

		return building;
	}

	std::shared_ptr<Building> BuildingSimplification::simplifyBuildingByCurve(const cv::Size& size, std::shared_ptr<util::Layer> layer, float alpha, float epsilon, float curve_threshold) {
		// to be implemented

		throw "Not implemented";

		return NULL;
	}

	/**
	 * Calculate cost function for the layer, which is defined by
	 *  \Sum_slice  [ \alpha * (1 - IOU) + (1 - \alpha) * simiplicity ] * area(slice)
	 */
	std::vector<float> BuildingSimplification::calculateCost(const cv::Size& size, const util::Polygon& simplified_polygon, std::shared_ptr<util::Layer> layer, float alpha) {
		std::vector<float> ans(4, 0);

		cv::Mat_<uchar> simplified_polygon_img;
		util::createImageFromPolygon(size.width, size.height, simplified_polygon, cv::Point(0, 0), simplified_polygon_img);

		// calculate IOU
		for (int i = 0; i < layer->slices.size(); i++) {
			float slice_area = util::calculateArea(layer->slices[i]);

			float iou = util::calculateIOU(simplified_polygon_img, layer->slices[i]);
			ans[0] += (1 - iou) * 2 * slice_area;
			ans[1] += slice_area;
		}

		// calculate the number of primitive shapes of the raw contour polygon as the baseline
		std::vector<util::Polygon> polygons = util::findContours(layer->selectRepresentativeSlice());
		int baseline_num_prim = 1;
		if (polygons.size() > 0) {
			OpenCVSimplification::decomposePolygon(polygons[0]);
			baseline_num_prim = polygons[0].primitive_shapes.size();
		}

		ans[2] = simplified_polygon.primitive_shapes.size();
		
		return ans;
	}

}