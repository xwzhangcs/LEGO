#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "../util/BuildingLayer.h"
#include "../util/DisjointVoxelData.h"
#include "../util/LayerVoxelData.h"

namespace simp {
	
	class BuildingSimplification {
	public:
		enum ALGORITHM { ALG_UNKNOWN = 0, ALG_ALL, ALG_DP, ALG_RIGHTANGLE, ALG_CURVE };

	protected:
		BuildingSimplification() {}

	public:
		static std::vector<std::shared_ptr<util::BuildingLayer>> simplifyBuildings(const util::DisjointVoxelData& disjointed_voxel_data, int ground_level, int algorithm, float alpha, float layering_threshold, float epsilon, int resolution, float curve_threshold);
		static std::vector<float> sumCost(const std::vector<std::shared_ptr<util::BuildingLayer>>& buildings);

	private:
		static std::shared_ptr<util::BuildingLayer> simplifyBuildingByAll(int building_id, const cv::Size& size, std::shared_ptr<util::Layer> layer, float alpha, float angle, int dx, int dy);
		static std::shared_ptr<util::BuildingLayer> simplifyBuildingByDP(int building_id, const cv::Size& size, std::shared_ptr<util::Layer> layer, float alpha, float epsilon);
		static std::shared_ptr<util::BuildingLayer> simplifyBuildingByOurCustom(int building_id, const cv::Size& size, std::shared_ptr<util::Layer> layer, float alpha, int resolution, float angle, int dx, int dy);
		static std::shared_ptr<util::BuildingLayer> simplifyBuildingByCurve(int building_id, const cv::Size& size, std::shared_ptr<util::Layer> layer, float alpha, float epsilon, float curve_threshold);

		static std::vector<float> calculateCost(const cv::Size& size, const util::Polygon& polygon, std::shared_ptr<util::Layer> layer, float alpha);
	};

}