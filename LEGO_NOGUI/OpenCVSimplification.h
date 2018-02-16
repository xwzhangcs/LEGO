#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "BuildingLayer.h"
#include "ContourUtils.h"

namespace simp {

	class OpenCVSimplification {
	protected:
		OpenCVSimplification() {}

	public:
		static util::Polygon simplify(const cv::Mat& slice, float epsilon);
		static void decomposePolygon(util::Polygon& polygon);
	};

}