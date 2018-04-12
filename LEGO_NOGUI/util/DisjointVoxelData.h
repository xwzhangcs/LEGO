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
		static std::vector<std::shared_ptr<BuildingLayer>> disjoint(const std::vector<cv::Mat_<uchar>>& voxel_data, int voxel_value_threshold = 128);
		static std::shared_ptr<BuildingLayer> layering(const std::shared_ptr<BuildingLayer>& building, float threshold);

	private:
		static int dfs3D(const std::vector<cv::Mat_<uchar>>&voxel_data, std::vector<cv::Mat_<unsigned short>>&clustering, int r, int c, int h, int cluster_id, int voxel_value_threshold);
		static cv::Mat_<uchar> getSliceOfCluster(const cv::Mat_<unsigned short>& cluster_data, int cluster_id, int& min_x, int& min_y, int& max_x, int& max_y);
		static void removeThinLayers(std::shared_ptr<BuildingLayer> layer, int min_num_slices_per_layer = 5);
	};

}