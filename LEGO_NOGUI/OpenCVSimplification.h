#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "Building.h"
#include "ContourUtils.h"

namespace simp {

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
		void calculateBuilding(Building* parent, const util::Polygon& polygon, int height, std::vector<Building>& buildings);
		Building calculateBuildingComponent(Building* parent, const util::Polygon& polygon, int bottom_height, int top_height);
		int findDrasticChange(int height, const util::Polygon& polygon, double threshold);
		util::Polygon simplifyPolygon(const util::Polygon& polygon, double epsilon);
	};

}