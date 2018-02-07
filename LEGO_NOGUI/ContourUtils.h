#pragma once

#include <vector>
#include <map>
#include <tuple>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace util {

	class Rectangle {
	public:
		cv::Mat_<float> mat;
		cv::Point2f min_pt;
		cv::Point2f max_pt;

	public:
		Rectangle();
		Rectangle(const cv::Mat_<float>& mat, const cv::Point2f& min_pt, const cv::Point2f& max_pt);
		std::vector<cv::Point2f> getActualPoints();
	};

	class Ring {
	public:
		cv::Mat_<float> mat;
		std::vector<cv::Point2f> points;

	public:
		Ring();

		const cv::Point2f& front() const;
		cv::Point2f& front();
		const cv::Point2f& back() const;
		cv::Point2f& back();
		std::vector<cv::Point2f>::iterator begin();
		std::vector<cv::Point2f>::iterator end();
		size_t size() const;
		void clear();
		void resize(size_t s);
		const cv::Point2f& operator[](int index) const;
		cv::Point2f& operator[](int index);
		void push_back(const cv::Point2f& pt);
		void pop_back();
		void erase(std::vector<cv::Point2f>::iterator position);
		void translate(float x, float y);
		void transform(const cv::Mat_<float>& m);
		void clockwise();
		void counterClockwise();
		cv::Point2f getActualPoint(int index);
		Ring getActualPoints();
	};

	class Polygon {
	public:
		cv::Mat_<float> mat;
		Ring contour;
		std::vector<Ring> holes;

		// For the right angle simplification, we currently store the decomposed primitive shapes (i.e., rectangles) in this data.
		// When generating a 3D geometry, or producing PLY file, this data should be used.
		std::vector<Rectangle> rectangles;

	public:
		Polygon() {}

		void translate(float x, float y);
		void transform(const cv::Mat_<float>& m);
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
	bool withinPolygon(const cv::Point2f& pt, const Polygon& polygon);
	bool withinPolygon(const cv::Point2f& pt, const Ring& ring);
	double calculateIOU(const cv::Mat_<uchar>& img, const cv::Mat_<uchar>& img2);
	double calculateIOU(const cv::Mat_<uchar>& img1, const cv::Mat_<uchar>& img2, const cv::Rect& rect);
	std::vector<Polygon> findContours(const cv::Mat_<uchar>& img);
	Ring addCornerToOpenCVContour(const std::vector<cv::Point>& polygon, const cv::Mat_<uchar>& img);
	void findContour(const cv::Mat_<uchar>& img, std::vector<cv::Point>& contour);
	void createImageFromContour(int width, int height, const std::vector<cv::Point>& contour, const cv::Point& offset, cv::Mat_<uchar>& result);
	void createImageFromPolygon(int width, int height, const Polygon& polygon, const cv::Point& offset, cv::Mat_<uchar>& result);

	void snapPolygon(const std::vector<cv::Point2f>& ref_polygon, std::vector<cv::Point2f>& polygon, float snap_vertex_threshold, float snap_edge_threshold);
	float dotProduct(const cv::Point2f& v1, const cv::Point2f& v2);
	float crossProduct(const cv::Point2f& v1, const cv::Point2f& v2);
	float closestPoint(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c, bool segmentOnly, cv::Point2f& pt);

}
