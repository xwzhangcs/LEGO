#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

namespace util {

	class DisjointVoxelData {
	public:
		std::vector<cv::Mat_<uchar>> voxel_data;
		std::vector<cv::Mat_<int>> clustered_voxel_data;
		int num_buildings;

	public:
		DisjointVoxelData();

		void disjoint(const std::vector<cv::Mat_<uchar>>& voxel_data, float threshold);
		size_t size() const { return num_buildings; }
		std::vector<cv::Mat_<uchar>> getDisjointedVoxelData(int building_id) const;

	private:
		void traverse(const std::vector<cv::Mat_<uchar>>& voxel_data, float threshold, std::vector<cv::Mat_<int>>& clustered_voxel_data, int cluster_id, int slice_id, int r, int c);
	};

}