#pragma once

#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include "ContourUtils.h"

namespace simp {

	class BuildingLayer {
	public:
		util::Polygon footprint;
		float bottom_height;
		float top_height;
		std::vector<float> costs;	// error, error_denominator, #primitives
		std::vector<std::shared_ptr<BuildingLayer>> children;

	public:
		BuildingLayer() {}
		BuildingLayer(const util::Polygon& footprint, float bottom_height, float top_height);
	};

}