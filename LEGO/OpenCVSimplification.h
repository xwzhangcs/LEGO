#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "Building.h"
#include "ContourUtils.h"

namespace lego {

	class OpenCVSimplification {
	private:
		std::vector<cv::Mat> voxel_data;
		double epsilon;
		double layering_threshold;
		double snap_vertex_threshold;
		double snap_edge_threshold;
		cv::Size size;

	public:
		OpenCVSimplification(const std::vector<cv::Mat>& voxel_data, double epsilon, double layering_threshold, double snap_vertex_threshold, double snap_edge_threshold);

		void simplify(std::vector<Building>& buildings);
		void calculateBuilding(Building* parent, const Polygon& polygon, int height, std::vector<Building>& buildings);
		Building calculateBuildingComponent(Building* parent, const Polygon& polygon, int bottom_height, int top_height);
		int findDrasticChange(int height, const Polygon& polygon, double threshold);
		Polygon simplifyPolygon(const Polygon& polygon, double epsilon);
	};

}