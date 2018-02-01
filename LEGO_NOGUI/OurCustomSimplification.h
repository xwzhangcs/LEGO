#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "Building.h"
#include "ContourUtils.h"

namespace simp {

	class OurCustomSimplification {
	private:
		std::vector<cv::Mat> voxel_data;
		double resolution;
		double layering_threshold;
		double snap_vertex_threshold;
		double snap_edge_threshold;
		cv::Size size;

	public:
		OurCustomSimplification(const std::vector<cv::Mat>& voxel_data, int resolution, double layering_threshold, double snap_vertex_threshold, double snap_edge_threshold);

		void simplify(std::vector<Building>& buildings);
		void calculateBuilding(Building* parent, const util::Polygon& polygon, int height, double angle, int dx, int dy, std::vector<Building>& buildings);
		Building calculateBuildingComponent(Building* parent, const util::Polygon& polygon, int bottom_height, int top_height, double& angle, int& dx, int& dy);
		int findDrasticChange(int height, const util::Polygon& polygon, double threshold);
		util::Polygon simplifyPolygon(const util::Polygon& polygon, double epsilon, double& angle, int& dx, int& dy);

		std::tuple<double, int, int> simplify(const std::vector<cv::Point2f>& contour, std::vector<cv::Point2f>& result, double resolution = 5.0);
		double simplify(const std::vector<cv::Point2f>& contour, std::vector<cv::Point2f>& result, double resolution, double angle, int dx, int dy);

		double optimizeSimplifiedContour(const std::vector<cv::Point>& contour, std::vector<cv::Point>& simplified_contour);
		std::vector<cv::Point> proposedContour(const std::vector<cv::Point>& contour, std::map<int, int>& x_map, std::map<int, int>& y_map);

	};

}