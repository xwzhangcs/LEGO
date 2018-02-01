#include "Building.h"

namespace simp {

	Building::Building(const std::vector<cv::Point2f>& footprint, float bottom_height, float top_height) {
		this->footprint = footprint;
		this->bottom_height = bottom_height;
		this->top_height = top_height;
	}

	Building::~Building() {
	}

}