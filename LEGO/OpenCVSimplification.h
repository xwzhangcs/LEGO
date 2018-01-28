#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include <QSize>
#include "Building.h"
#include "BuildingSimplification.h"

class OpenCVSimplification : public BuildingSimplification {
private:
	std::vector<cv::Mat> voxel_data;
	double epsilon;
	double slicing_threshold;
	QSize size;

public:
	OpenCVSimplification(const std::vector<cv::Mat>& voxel_data, double epsilon, double slicing_threshold);

	void simplify(std::vector<Building>& buildings);
	void calculateBuilding(const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, int height, std::vector<Building>& buildings);
	Building calculateBuildingComponent(const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, int bottom_height, int top_height);
	int findDrasticChange(int height, const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, double threshold);

};

