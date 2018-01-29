#pragma once

#include <vector>
#include <map>
#include <tuple>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace contour {

	std::tuple<double, int, int> simplify(const std::vector<cv::Point>& contour, std::vector<cv::Point2f>& result, double resolution = 5.0);
	double simplify(const std::vector<cv::Point>& contour, std::vector<cv::Point2f>& result, double resolution, double angle, int dx, int dy);
	void findContour(const cv::Mat& img, std::vector<cv::Point>& contour);

	double optimizeSimplifiedContour(const std::vector<cv::Point>& contour, std::vector<cv::Point>& simplified_contour);
	void createImageFromContour(int width, int height, const std::vector<cv::Point>& contour, int min_x, int min_y, cv::Mat& result);
	double calculateIOU(const cv::Mat& img, const cv::Mat& img2);
	std::vector<cv::Point> proposedContour(const std::vector<cv::Point>& contour, std::map<int, int>& x_map, std::map<int, int>& y_map);

	std::vector<cv::Point> removeRedundantPoint(const std::vector<cv::Point>& polygon);
	std::vector<cv::Point2f> removeRedundantPoint(const std::vector<cv::Point2f>& polygon);
}
