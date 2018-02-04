#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "Building.h"
#include "ContourUtils.h"

namespace simp {

	class OurCustomSimplification {
	protected:
		OurCustomSimplification() {}

	public:
		static util::Polygon simplify(const cv::Mat& slice, int resolution, float& angle, int& dx, int& dy);

	private:
		static std::tuple<float, int, int> simplifyContour(const std::vector<cv::Point2f>& contour, std::vector<cv::Point2f>& result, int resolution = 5.0);
		static double simplifyContour(const std::vector<cv::Point2f>& contour, std::vector<cv::Point2f>& result, int resolution, float angle, int dx, int dy);

		static double optimizeSimplifiedContour(const std::vector<cv::Point>& contour, std::vector<cv::Point>& simplified_contour);
		static std::vector<cv::Point> proposedContour(const std::vector<cv::Point>& contour, std::map<int, int>& x_map, std::map<int, int>& y_map);

	};

}