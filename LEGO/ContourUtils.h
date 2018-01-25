#pragma once

#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace contour {

	void simplify(std::vector<cv::Point> contour, std::vector<cv::Point2f>& result, double eps = 1.0);
	void regularizePolygon(std::vector<cv::Point> contour, std::vector<cv::Point2f>& result);
	void findContour(const cv::Mat& img, std::vector<cv::Point2f>& contour);

}
