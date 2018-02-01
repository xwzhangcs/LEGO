#pragma once

#include <vector>
#include <map>
#include <tuple>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace util {

	class Polygon {
	public:
		std::vector<cv::Point> contour;
		std::vector<std::vector<cv::Point>> holes;

	public:
		Polygon() {}

		void translate(int x, int y);
	};

	std::vector<cv::Point> removeRedundantPoint(const std::vector<cv::Point>& polygon);
	std::vector<cv::Point2f> removeRedundantPoint(const std::vector<cv::Point2f>& polygon);

	cv::Rect boundingBox(const std::vector<cv::Point>& polygon);
	double calculateIOU(const cv::Mat& img, const cv::Mat& img2);
	double calculateIOU(const cv::Mat& img1, const cv::Mat& img2, const cv::Rect& rect);
	std::vector<Polygon> findContours(const cv::Mat& img);
	void findContour(const cv::Mat& img, std::vector<cv::Point>& contour);
	void createImageFromContour(int width, int height, const std::vector<cv::Point>& contour, const cv::Point& offset, cv::Mat& result);
	void createImageFromPolygon(int width, int height, const Polygon& polygon, const cv::Point& offset, cv::Mat& result);

	void snapPolygon(const std::vector<cv::Point2f>& ref_polygon, std::vector<cv::Point2f>& polygon, float snap_vertex_threshold, float snap_edge_threshold);
	float dotProduct(const cv::Point2f& v1, const cv::Point2f& v2);
	float crossProduct(const cv::Point2f& v1, const cv::Point2f& v2);
	float closestPoint(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c, bool segmentOnly, cv::Point2f& pt);

}
