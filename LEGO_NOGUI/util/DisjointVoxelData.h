#pragma once

#include <vector>
#include <unordered_map>
#include <opencv2/opencv.hpp>
#include "BuildingLayer.h"

namespace util {

	class DisjointVoxelData {
	protected:
		DisjointVoxelData();

	public:
		static std::vector<std::shared_ptr<BuildingLayer>> disjoint(const std::vector<cv::Mat_<uchar>>& voxel_data);
		static std::shared_ptr<BuildingLayer> layering(const std::shared_ptr<BuildingLayer>& building, float threshold);

	private:
		static void dfs3D(const std::vector<cv::Mat_<uchar>>&voxel_data, std::vector<cv::Mat_<unsigned short>>&clustering, int r, int c, int h, int cluster_id);
		static cv::Mat_<uchar> getSliceOfCluster(const cv::Mat_<unsigned short>& cluster_data, int cluster_id, int& min_x, int& min_y, int& max_x, int& max_y);
	};

}