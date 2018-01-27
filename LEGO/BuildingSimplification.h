#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

class BuildingSimplification {
public:
	BuildingSimplification();

	static cv::Rect boundingBox(const std::vector<cv::Point>& polygon);
	static double calculateIOU(const cv::Mat& img1, const cv::Mat& img2, const cv::Rect& rect);
};

