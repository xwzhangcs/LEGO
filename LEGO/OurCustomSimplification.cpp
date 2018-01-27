#include "OurCustomSimplification.h"
#include "ContourUtils.h"

OurCustomSimplification::OurCustomSimplification(const std::vector<cv::Mat>& voxel_data, int resolution, double slicing_threshold) {
	this->voxel_data = voxel_data;
	this->resolution = resolution;
	this->slicing_threshold = slicing_threshold;
	this->size = QSize(voxel_data[0].cols, voxel_data[0].rows);
}

void OurCustomSimplification::simplify(std::vector<Building>& buildings) {
	// extract contours
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(voxel_data[5].clone(), contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	// traverse all the external contours
	for (int i = 0; i < hierarchy.size(); i++) {
		std::cout << "Processing building " << i << std::endl;

		if (hierarchy[i][3] != -1) continue;
		if (contours[i].size() < 3) continue;

		// obtain all the holes inside this contour
		std::vector<std::vector<cv::Point>> holes;
		int hole_id = hierarchy[i][2];
		while (hole_id != -1) {
			holes.push_back(contours[hole_id]);

			hole_id = hierarchy[hole_id][0];
		}

		calculateBuildings(contours[i], holes, 5, buildings);
	}
	std::cout << "Processing buildings has been finished." << std::endl;
}

void OurCustomSimplification::calculateBuildings(const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, int height, std::vector<Building>& buildings) {
	// calculate the bounding box
	cv::Rect bbox = boundingBox(contour);

	// have 1px as margin
	bbox.x = std::max(0, bbox.x - 1);
	bbox.y = std::max(0, bbox.y - 1);
	bbox.width = std::min(size.width() - bbox.x - 1, bbox.width + 1);
	bbox.height = std::min(size.height() - bbox.y - 1, bbox.height + 1);

	// find the height at which the contour drastically changes
	int next_height = findDrasticChange(height, contour, holes, slicing_threshold);

	// calculate building by simplifying the contour and holes
	try {
		Building building = calculateBuilding(contour, holes, height, next_height);
		buildings.push_back(building);

		if (next_height >= voxel_data.size()) return;

		cv::Mat next_img(voxel_data[next_height], bbox);

		// extract contours
		std::vector<std::vector<cv::Point>> next_contours;
		std::vector<cv::Vec4i> next_hierarchy;
		cv::findContours(next_img.clone(), next_contours, next_hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

		for (int i = 0; i < next_hierarchy.size(); i++) {
			if (next_hierarchy[i][3] != -1) continue;
			if (next_contours[i].size() < 3) continue;

			std::vector<cv::Point> next_contour = next_contours[i];

			// obtain all the holes inside this contour
			std::vector<std::vector<cv::Point>> next_holes;
			int hole_id = next_hierarchy[i][2];
			while (hole_id != -1) {
				next_holes.push_back(next_contours[hole_id]);
				hole_id = next_hierarchy[hole_id][0];
			}

			// offset back the contour and holes
			for (int j = 0; j < next_contour.size(); j++) {
				next_contour[j].x += bbox.x;
				next_contour[j].y += bbox.y;
			}
			for (int j = 0; j < next_holes.size(); j++) {
				for (int k = 0; k < next_holes[j].size(); k++) {
					next_holes[j][k].x += bbox.x;
					next_holes[j][k].y += bbox.y;
				}
			}

			// check if the next contour is mostly within the contour
			int cnt_outside = 0;
			for (int j = 0; j < next_contour.size(); j++) {
				if (pointPolygonTest(contour, next_contour[j], false) < 0) cnt_outside++;
			}

			if (cnt_outside < next_contour.size() / 2) {
				calculateBuildings(next_contour, next_holes, next_height, buildings);
			}
		}
	}
	catch (char* ex) {
	}
}

/**
* Calculate the building geometry by simplifying the specified footprint and holes using OpenCV function.
*/
Building OurCustomSimplification::calculateBuilding(const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, int bottom_height, int top_height) {
	std::vector<cv::Point2f> simplified_contour;
	contour::simplify(contour, simplified_contour, resolution);

	if (simplified_contour.size() < 3) throw "Invalid contour";

	// create a building object
	Building building;
	building.bottom_height = bottom_height;
	building.top_height = top_height;

	building.footprint.resize(simplified_contour.size());
	for (int i = 0; i < simplified_contour.size(); i++) {
		building.footprint[i] = glm::dvec2(simplified_contour[i].x - size.width() * 0.5, size.height() * 0.5 - simplified_contour[i].y);
	}

	// simplify the hole as well
	building.holes.resize(holes.size());
	for (int i = 0; i < holes.size(); i++) {
		std::vector<cv::Point2f> simplified_hole;
		std::vector<cv::Point2f> simplified_hole2;
		contour::simplify(holes[i], simplified_hole, resolution);
		if (simplified_hole.size() >= 3) {
			building.holes[i].resize(simplified_hole.size());
			for (int j = 0; j < simplified_hole.size(); j++) {
				building.holes[i][j] = glm::dvec2(simplified_hole[j].x - size.width() * 0.5, size.height() * 0.5 - simplified_hole[j].y);
			}
		}
	}

	return building;
}

int OurCustomSimplification::findDrasticChange(int height, const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, double threshold) {
	// calculate the bounding box
	cv::Rect bbox = boundingBox(contour);

	// create image of the contour of the current slice
	cv::Mat img(size.height(), size.width(), CV_8U, cv::Scalar(0));
	std::vector<std::vector<cv::Point>> contours(1);
	contours[0] = contour;
	cv::fillPoly(img, contours, cv::Scalar(255), cv::LINE_4);
	for (int i = 0; i < holes.size(); i++) {
		std::vector<std::vector<cv::Point>> hole_pts(1);
		hole_pts[0] = holes[i];
		cv::fillPoly(img, hole_pts, cv::Scalar(0), cv::LINE_4);
	}

	for (int i = height + 1; i < voxel_data.size(); i++) {
		double iou = calculateIOU(img, voxel_data[i], bbox);
		if (iou < threshold) {
			return i;
		}
	}

	return voxel_data.size();
}

