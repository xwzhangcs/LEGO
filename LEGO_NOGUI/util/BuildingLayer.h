#pragma once

#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include "ContourUtils.h"

namespace util {

	class BuildingLayer {
	public:
		int building_id;
		util::Polygon footprint;
		float bottom_height;
		float top_height;
		std::vector<float> costs;	// error, error_denominator, #primitives
		std::vector<std::shared_ptr<BuildingLayer>> children;

	public:
		BuildingLayer() {}
		BuildingLayer(int building_id, const util::Polygon& footprint, float bottom_height, float top_height);
	};

}