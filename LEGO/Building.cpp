#include "Building.h"


Building::Building(const std::vector<glm::dvec2>& footprint, double bottom_height, double top_height) {
	this->footprint = footprint;
	this->bottom_height = bottom_height;
	this->top_height = top_height;
}

Building::~Building() {
}
