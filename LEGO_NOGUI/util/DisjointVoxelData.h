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
		static cv::Mat_<uchar> getSliceOfCluster(const std::vector<cv::Mat_<uchar>>& voxel_data, int slice_id, const cv::Mat_<unsigned short>& cluster_data, int cluster_id);
		static void traverseInSlice(const std::vector<cv::Mat_<uchar>>& voxel_data, cv::Mat_<unsigned short>& cluster, int cluster_id, int slice_id, int r, int c);
	};

}