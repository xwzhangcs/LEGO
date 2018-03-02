#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "../util/BuildingLayer.h"
#include "../util/ContourUtils.h"

namespace simp {

	class RightAngleSimplification {
	protected:
		RightAngleSimplification() {}

	public:
		static util::Polygon simplify(const util::Polygon& polygon, int resolution, float& angle, int& dx, int& dy);
		static void decomposePolygon(util::Polygon& polygon);

	private:
		static std::tuple<float, int, int> simplifyContour(const util::Ring& contour, util::Ring& result, int resolution = 5.0);
		static double simplifyContour(const util::Ring& contour, util::Ring& result, int resolution, float angle, int dx, int dy, bool refine);

		static double optimizeSimplifiedContour(const std::vector<cv::Point>& contour, std::vector<cv::Point>& simplified_contour);
		static std::vector<cv::Point> proposedContour(const std::vector<cv::Point>& contour, std::map<int, int>& x_map, std::map<int, int>& y_map);

		static void findMaximumRectangle(const std::vector<std::vector<bool>>& grid, const std::vector<float>& x_coords, const std::vector<float>& y_coords, int& x, int& y, int& width, int& height);
	};

}