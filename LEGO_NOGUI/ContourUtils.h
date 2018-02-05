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
		std::vector<cv::Point2f> contour;
		std::vector<std::vector<cv::Point2f>> holes;

	public:
		Polygon() {}

		void translate(float x, float y);
		void transform(cv::Mat m);
		void clockwise();
		void counterClockwise();
	};

	bool isClockwise(const std::vector<cv::Point2f>& polygon);
	void clockwise(std::vector<cv::Point2f>& polygon);
	void counterClockwise(std::vector<cv::Point2f>& polygon);
	
	std::vector<cv::Point> removeRedundantPoint(const std::vector<cv::Point>& polygon);
	std::vector<cv::Point2f> removeRedundantPoint(const std::vector<cv::Point2f>& polygon);

	cv::Rect boundingBox(const std::vector<cv::Point>& polygon);
	cv::Rect boundingBox(const std::vector<cv::Point2f>& polygon);
	double calculateIOU(const cv::Mat_<uchar>& img, const cv::Mat_<uchar>& img2);
	double calculateIOU(const cv::Mat_<uchar>& img1, const cv::Mat_<uchar>& img2, const cv::Rect& rect);
	std::vector<Polygon> findContours(const cv::Mat_<uchar>& img);
	std::vector<cv::Point2f> addCornerToOpenCVContour(const std::vector<cv::Point>& polygon, const cv::Mat_<uchar>& img);
	void findContour(const cv::Mat_<uchar>& img, std::vector<cv::Point>& contour);
	void createImageFromContour(int width, int height, const std::vector<cv::Point>& contour, const cv::Point& offset, cv::Mat_<uchar>& result);
	void createImageFromPolygon(int width, int height, const Polygon& polygon, const cv::Point& offset, cv::Mat_<uchar>& result);

	void snapPolygon(const std::vector<cv::Point2f>& ref_polygon, std::vector<cv::Point2f>& polygon, float snap_vertex_threshold, float snap_edge_threshold);
	float dotProduct(const cv::Point2f& v1, const cv::Point2f& v2);
	float crossProduct(const cv::Point2f& v1, const cv::Point2f& v2);
	float closestPoint(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c, bool segmentOnly, cv::Point2f& pt);

}
