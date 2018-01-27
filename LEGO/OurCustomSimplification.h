#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include <QSize>
#include "Building.h"
#include "GLUtils.h"

class OurCustomSimplification {
private:
	std::vector<cv::Mat> voxel_data;
	double resolution;
	double slicing_threshold;
	QSize size;

public:
	OurCustomSimplification(const std::vector<cv::Mat>& voxel_data, int resolution, double slicing_threshold);

	void simplify(std::vector<Building>& buildings);
	void calculateBuildings(std::vector<cv::Point> contour, std::vector<std::vector<cv::Point>> holes, int height, std::vector<Building>& buildings);
	Building calculateBuilding(const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, int bottom_height, int top_height);
	int findDrasticChange(int height, const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, double threshold);

};

