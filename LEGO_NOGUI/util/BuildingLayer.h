#pragma once

#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include "ContourUtils.h"

namespace util {

	class BuildingLayer {
	public:
		int building_id;
		std::vector<util::Polygon> raw_footprints;
		util::Polygon footprint;
		float bottom_height;
		float top_height;
		std::vector<float> costs;	// error, error_denominator, #primitives
		std::vector<std::shared_ptr<BuildingLayer>> children;

	public:
		BuildingLayer() {}
		BuildingLayer(int building_id, float bottom_height, float top_height);
		BuildingLayer(int building_id, const util::Polygon& footprint, float bottom_height, float top_height);

		util::Polygon selectRepresentativeContour() const;
	};

}