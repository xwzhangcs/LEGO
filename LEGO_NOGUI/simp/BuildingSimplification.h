#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "../util/BuildingLayer.h"
#include "../util/DisjointVoxelData.h"

namespace simp {
	
	class BuildingSimplification {
	public:
		enum ALGORITHM { ALG_UNKNOWN = 0, ALG_ALL, ALG_DP, ALG_RIGHTANGLE, ALG_CURVE, ALG_CURVE_RIGHTANGLE };

	protected:
		BuildingSimplification() {}

	public:
		static std::vector<std::shared_ptr<util::BuildingLayer>> simplifyBuildings(const std::vector<std::shared_ptr<util::BuildingLayer>>& raw_buildings, int algorithm, bool record_stats, int min_num_slices_per_layer, float alpha, float layering_threshold, float epsilon, int resolution, float curve_threshold, float angle_threshold);

	private:
		static std::shared_ptr<util::BuildingLayer> simplifyBuildingByAll(int building_id, std::shared_ptr<util::BuildingLayer> layer, float alpha, float angle, int dx, int dy, std::vector<std::tuple<float, long long, int>>& records);
		static std::shared_ptr<util::BuildingLayer> simplifyBuildingByDP(int building_id, std::shared_ptr<util::BuildingLayer> layer, float alpha, float epsilon, std::vector<std::tuple<float, long long, int>>& records);
		static std::shared_ptr<util::BuildingLayer> simplifyBuildingByRightAngle(int building_id, std::shared_ptr<util::BuildingLayer> layer, float alpha, int resolution, float angle, int dx, int dy, std::vector<std::tuple<float, long long, int>>& records);
		static std::shared_ptr<util::BuildingLayer> simplifyBuildingByCurve(int building_id, std::shared_ptr<util::BuildingLayer> layer, float alpha, float epsilon, float curve_threshold, std::vector<std::tuple<float, long long, int>>& records);
		static std::shared_ptr<util::BuildingLayer> simplifyBuildingByCurveRightAngle(int building_id, std::shared_ptr<util::BuildingLayer> layer, float alpha, float epsilon, float curve_threshold, float angle_threshold, std::vector<std::tuple<float, long long, int>>& records);

		static std::vector<float> calculateCost(const util::Polygon& simplified_polygon, const util::Polygon& polygon, int height);
	};

}