#include "Building.h"


Building::Building(const std::vector<glm::dvec2>& footprint, double height) {
	this->footprint = footprint;
	this->height = height;
}

Building::~Building() {
}
