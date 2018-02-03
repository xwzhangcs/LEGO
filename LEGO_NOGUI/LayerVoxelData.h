#pragma once

#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>

namespace util {

	class Layer {
	public:
		int bottom_height;
		int top_height;
		std::vector<cv::Mat> slices;
		std::vector<std::shared_ptr<Layer>> children;

	public:
		Layer() {}
		Layer(int bottom_height, int top_height) : bottom_height(bottom_height), top_height(top_height) {}
	};

	class LayerVoxelData {
	private:
		int threshold;
		std::vector<cv::Mat_<uchar>> voxel_data;

	public:
		LayerVoxelData(const std::vector<cv::Mat_<uchar>>& voxel_data, float threshold);

		std::shared_ptr<Layer> layering(float layering_threshold);

	private:
		void layering(std::shared_ptr<Layer> current_layer, const cv::Mat_<uchar>& current_slice, int height, const cv::Mat_<uchar>& bottom_slice, float layering_threshold);
		void createSlice(cv::Mat slice, cv::Mat_<int> cluster, int cluster_id, cv::Mat& next_slice);
		void traverse(cv::Mat_<int>& cluster, int cluster_id, const cv::Mat_<uchar>& slice, int r, int c);
		int countVoxel(const cv::Mat& slice);
	};

}