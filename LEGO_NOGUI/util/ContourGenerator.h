#pragma once

#include <vector>
#include <memory>
#include "PrimitiveShape.h"
#include "ContourUtils.h"


class ContourGenerator {
protected:
	ContourGenerator();
	~ContourGenerator();

public:
	static void generate(const util::Polygon& polygon, const std::vector<std::pair<int, std::shared_ptr<efficient_ransac::PrimitiveShape>>>& shapes, std::vector<cv::Point2f>& contour, float max_error, float angle_threshold);
	static void applyHeuristics(const std::vector<cv::Point2f>& polygon, std::vector<cv::Point2f>& contour, float max_error, float angle_threshold);
};

