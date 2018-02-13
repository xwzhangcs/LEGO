#pragma once

#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include "ContourUtils.h"

namespace simp {

	class Building {
	public:
		util::Polygon footprint;
		float bottom_height;
		float top_height;
		std::vector<float> costs;	// error, error_denominator, #primitives
		std::vector<std::shared_ptr<Building>> children;

	public:
		Building() {}
		Building(const util::Polygon& footprint, float bottom_height, float top_height);
		~Building();
	};

}