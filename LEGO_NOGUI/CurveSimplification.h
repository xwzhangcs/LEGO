#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "BuildingLayer.h"
#include "ContourUtils.h"

namespace simp {

	class CurveSimplification {
	protected:
		CurveSimplification() {}

	public:
		static util::Polygon simplify(const cv::Mat& slice, float epsilon, float curve_threshold);
		static void decomposePolygon(util::Polygon input, util::Polygon& polygon, float epsilon, float curve_threshold);
		static std::vector<util::Polygon> findContours(const cv::Mat_<uchar>& img, float epsilon, float curve_threshold);
		static int approxContour(std::vector<cv::Point2f>& input, util::Polygon &output, float epsilon, float curve_threshold);
		static bool concaveCurve(std::vector<cv::Point2f>& end_points, util::Polygon &polygon);
	};

}