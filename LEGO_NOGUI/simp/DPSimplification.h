#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "../util/BuildingLayer.h"
#include "../util/ContourUtils.h"

namespace simp {

	class DPSimplification {
	protected:
		DPSimplification() {}

	public:
		static util::Polygon simplify(const cv::Mat& slice, float epsilon);
		static void decomposePolygon(util::Polygon& polygon);
	};

}