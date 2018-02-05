#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "Building.h"
#include "LayerVoxelData.h"

namespace simp {
	
	class BuildingSimplification {
	public:
		static enum { ALG_OPENCV = 0, ALG_RIGHTANGLE, ALG_CURVE };

	private:
		cv::Size size;
		std::vector<std::vector<cv::Mat_<uchar>>> disjointed_voxel_data;
		float layering_threshold;
		float snap_vertex_threshold;
		float snap_edge_threshold;
		int epsilon;
		int resolution;

	public:
		BuildingSimplification(const std::vector<std::vector<cv::Mat_<uchar>>>& disjointed_voxel_data, float layering_threshold, float snap_vertex_threshold, float snap_edge_threshold, int epsilon, int resolution);

		std::vector<std::shared_ptr<Building>> simplifyBuildings(int algorithm);

	private:
		std::shared_ptr<Building> simplifyBuildingByOpenCV(std::shared_ptr<util::Layer> layer, float epsilon);
		std::shared_ptr<Building> simplifyBuildingByOurCustom(std::shared_ptr<util::Layer> layer, int resolution, float angle, int dx, int dy);
	};

}