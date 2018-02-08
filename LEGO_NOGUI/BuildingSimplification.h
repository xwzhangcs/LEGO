#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "Building.h"
#include "DisjointVoxelData.h"
#include "LayerVoxelData.h"

namespace simp {
	
	class BuildingSimplification {
	public:
		enum ALGORITHM { ALG_ALL = 0, ALG_OPENCV, ALG_RIGHTANGLE, ALG_CURVE };

	protected:
		BuildingSimplification() {}

	public:
		static std::vector<std::shared_ptr<Building>> simplifyBuildings(const util::DisjointVoxelData& disjointed_voxel_data, int algorithm, float layering_threshold, float snap_vertex_threshold, float snap_edge_threshold, int epsilon, int resolution, float curve_threshold);

	private:
		static std::shared_ptr<Building> simplifyBuildingByAll(const cv::Size& size, std::shared_ptr<util::Layer> layer);
		static std::shared_ptr<Building> simplifyBuildingByOpenCV(const cv::Size& size, std::shared_ptr<util::Layer> layer, float snap_vertex_threshold, float snap_edge_threshold, float epsilon);
		static std::shared_ptr<Building> simplifyBuildingByOurCustom(const cv::Size& size, std::shared_ptr<util::Layer> layer, float snap_vertex_threshold, float snap_edge_threshold, int resolution, float angle, int dx, int dy);
		static std::shared_ptr<Building> simplifyBuildingByCurve(const cv::Size& size, std::shared_ptr<util::Layer> layer, float snap_vertex_threshold, float snap_edge_threshold, float epsilon, float curve_threshold);

		static float calculateCost(const util::Polygon& polygon, std::shared_ptr<util::Layer> layer);
	};

}