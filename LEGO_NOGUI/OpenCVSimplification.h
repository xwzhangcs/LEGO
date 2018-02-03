#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "Building.h"
#include "ContourUtils.h"

namespace simp {

	class OpenCVSimplification {
	protected:
		OpenCVSimplification() {}

	public:
		static util::Polygon simplify(const std::vector<cv::Mat>& slices, float epsilon);

	private:
		static util::Polygon simplifyPolygon(const util::Polygon& polygon, double epsilon);
	};

}