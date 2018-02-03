#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

namespace util {

	class DisjointVoxelData {
	protected:
		DisjointVoxelData() {}

	public:
		static std::vector<std::vector<cv::Mat_<uchar>>> disjoint(const std::vector<cv::Mat_<uchar>>& voxel_data, float threshold);

	private:
		static void traverse(const std::vector<cv::Mat_<uchar>>& voxel_data, float threshold, std::vector<cv::Mat_<int>>& clustered_voxel_data, int cluster_id, int slice_id, int r, int c);
	};

}