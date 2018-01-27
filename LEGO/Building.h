#pragma once

#include <vector>
#include <glm/glm.hpp>

class Building {
public:
	std::vector<glm::dvec2> footprint;
	std::vector<std::vector<glm::dvec2>> holes;
	double bottom_height;
	double top_height;

public:
	Building() {}
	Building(const std::vector<glm::dvec2>& footprint, double bottom_height, double top_height);
	~Building();
};

