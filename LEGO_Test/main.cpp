#include <iostream>
#include "util/ContourUtils.h"

void testFindContour(const char* filename) {
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "findContour testing..." << std::endl;
	std::cout << filename << std::endl;
	cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);
	std::vector<util::Polygon> polygons = util::findContours(img);

	if (polygons.size() == 0) {
		std::cout << "no polygon is found." << std::endl;
	}
	else {
		for (int i = 0; i < polygons.size(); i++) {
			std::cout << "Polygon " << i + 1 << ":" << std::endl;
			for (int j = 0; j < polygons[i].contour.size(); j++) {
				std::cout << "(" << polygons[i].contour[j].x << "," << polygons[i].contour[j].y << ") ";
			}
			std::cout << std::endl;
			for (int j = 0; j < polygons[i].holes.size(); j++) {
				std::cout << "  hole " << j + 1 << ":" << std::endl << "  ";
				for (int k = 0; k < polygons[i].holes[j].size(); k++) {
					std::cout << "(" << polygons[i].holes[j][k].x << "," << polygons[i].holes[j][k].y << ") ";
				}
				std::cout << std::endl;
			}
		}
	}
}

int main() {
	testFindContour("contour_test1.png");
	testFindContour("contour_test2.png");
	testFindContour("contour_test3.png");
	testFindContour("contour_test4.png");
	testFindContour("contour_test5.png");
	testFindContour("contour_test6.png");
	testFindContour("contour_test7.png");
	testFindContour("contour_test8.png");


	return 0;
}