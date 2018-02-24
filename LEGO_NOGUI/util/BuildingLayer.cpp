#include "BuildingLayer.h"

namespace util {

	BuildingLayer::BuildingLayer(int building_id, const util::Polygon& footprint, float bottom_height, float top_height) {
		this->building_id = building_id;
		this->footprint = footprint;
		this->bottom_height = bottom_height;
		this->top_height = top_height;
	}

}