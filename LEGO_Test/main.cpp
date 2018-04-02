#include <iostream>
#include "util/ContourUtils.h"
#include "simp/RightAngleSimplification.h"

void testApproxPolyDP(const char* filename) {
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "approxPolyDP testing..." << std::endl;
	std::cout << filename << std::endl;

	cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);
	std::vector<util::Polygon> polygons = util::findContours(img, false);

	util::Polygon simplified_polygon;
	util::approxPolyDP(polygons[0].contour.points, simplified_polygon.contour.points, 2, true, true);

	for (int i = 0; i < simplified_polygon.contour.size(); i++) {
		std::cout << "(" << simplified_polygon.contour[i].x << "," << simplified_polygon.contour[i].y << ")" << std::endl;
	}
}

void testFindContour(const char* filename) {
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "findContour testing..." << std::endl;
	std::cout << filename << std::endl;
	cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);
	std::vector<util::Polygon> polygons = util::findContours(img, false);

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

void testSimplification(const char* filename) {
	cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);

	// extract contours
	std::vector<util::Polygon> contours = util::findContours(img, false);

	for (int i = 0; i < contours.size(); i++) {
		try {
			float angle = -1;
			int dx = -1;
			int dy = -1;
			util::Polygon simplified_polygon = simp::RightAngleSimplification::simplify(contours[i], 12, angle, dx, dy);

			cv::Mat_<uchar> result;
			util::createImageFromPolygon(img.cols, img.rows, simplified_polygon, cv::Point(0, 0), result);
			char result_filename[255];
			sprintf(result_filename, "%s_result.png", filename);
			cv::imwrite(result_filename, result);
		}
		catch (...) {}
	}
}

int main() {
	testApproxPolyDP("complex_contour.png");

	testFindContour("contour_test1.png");
	testFindContour("contour_test2.png");
	testFindContour("contour_test3.png");
	testFindContour("contour_test4.png");
	testFindContour("contour_test5.png");
	testFindContour("contour_test6.png");
	testFindContour("contour_test7.png");
	testFindContour("contour_test8.png");
	
	testSimplification("simplify_test1.png");
	testSimplification("simplify_test2.png");
	testSimplification("simplify_test3.png");

	return 0;
}