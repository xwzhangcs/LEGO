#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "Building.h"
#include "DisjointVoxelData.h"
#include "LayerVoxelData.h"

namespace simp {
	
	class BuildingSimplification {
	public:
		enum ALGORITHM { ALG_UNKNOWN = 0, ALG_ALL, ALG_OPENCV, ALG_RIGHTANGLE, ALG_CURVE };

	protected:
		BuildingSimplification() {}

	public:
		static std::vector<std::shared_ptr<Building>> simplifyBuildings(const util::DisjointVoxelData& disjointed_voxel_data, int ground_level, int algorithm, float alpha, float layering_threshold, float epsilon, int resolution, float curve_threshold);
		static std::vector<float> sumCost(const std::vector<std::shared_ptr<Building>>& buildings);

	private:
		static std::shared_ptr<Building> simplifyBuildingByAll(const cv::Size& size, std::shared_ptr<util::Layer> layer, float alpha, float angle, int dx, int dy);
		static std::shared_ptr<Building> simplifyBuildingByOpenCV(const cv::Size& size, std::shared_ptr<util::Layer> layer, float alpha, float epsilon);
		static std::shared_ptr<Building> simplifyBuildingByOurCustom(const cv::Size& size, std::shared_ptr<util::Layer> layer, float alpha, int resolution, float angle, int dx, int dy);
		static std::shared_ptr<Building> simplifyBuildingByCurve(const cv::Size& size, std::shared_ptr<util::Layer> layer, float alpha, float epsilon, float curve_threshold);

		static std::vector<float> calculateCost(const cv::Size& size, const util::Polygon& polygon, std::shared_ptr<util::Layer> layer, float alpha);
	};

}