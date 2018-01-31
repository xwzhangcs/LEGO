#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

class Building {
public:
	std::vector<cv::Point2f> footprint;
	std::vector<std::vector<cv::Point2f>> holes;
	float bottom_height;
	float top_height;

public:
	Building() {}
	Building(const std::vector<cv::Point2f>& footprint, float bottom_height, float top_height);
	~Building();
};

