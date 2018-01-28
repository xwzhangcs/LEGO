#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include <QSize>
#include "Building.h"
#include "GLUtils.h"
#include "BuildingSimplification.h"

class OurCustomSimplification : public BuildingSimplification {
private:
	std::vector<cv::Mat> voxel_data;
	double resolution;
	double slicing_threshold;
	QSize size;

public:
	OurCustomSimplification(const std::vector<cv::Mat>& voxel_data, int resolution, double slicing_threshold);

	void simplify(std::vector<Building>& buildings);
	void calculateBuilding(const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, int height, double angle, int dx, int dy, std::vector<Building>& buildings);
	Building calculateBuildingComponent(const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, int bottom_height, int top_height, double& angle, int& dx, int& dy);
	int findDrasticChange(int height, const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, double threshold);

};

