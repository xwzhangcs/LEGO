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

void OurCustomSimplification::calculateBuildings(std::vector<cv::Point> contour, std::vector<std::vector<cv::Point>> holes, int height, std::vector<Building>& buildings) {
	// calculate the bounding box
	int min_x = std::numeric_limits<int>::max();
	int max_x = -std::numeric_limits<int>::max();
	int min_y = std::numeric_limits<int>::max();
	int max_y = -std::numeric_limits<int>::max();
	for (int i = 0; i < contour.size(); i++) {
		min_x = std::min(min_x, contour[i].x);
		max_x = std::max(max_x, contour[i].x);
		min_y = std::min(min_y, contour[i].y);
		max_y = std::max(max_y, contour[i].y);
	}

	// have 1px as margin
	min_x = std::max(0, min_x - 1);
	min_y = std::max(0, min_y - 1);
	max_x = std::min(size.width() - 1, max_x + 1);
	max_y = std::min(size.height() - 1, max_y + 1);

	// find the height at which the contour drastically changes
	int next_height = findDrasticChange(height, contour, holes, slicing_threshold);

	// calculate building by simplifying the contour and holes
	try {
		Building building = calculateBuilding(contour, holes, height, next_height);
		buildings.push_back(building);

		if (next_height >= voxel_data.size()) return;

		cv::Mat cropped_img(voxel_data[next_height], cv::Rect(min_x, min_y, max_x - min_x + 1, max_y - min_y + 1));

		// extract contours
		std::vector<std::vector<cv::Point>> contours_in_copped_img;
		std::vector<cv::Vec4i> hierarchy_in_cropped_img;
		cv::findContours(cropped_img.clone(), contours_in_copped_img, hierarchy_in_cropped_img, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

		bool found_next_contour = false;
		for (int i = 0; i < hierarchy_in_cropped_img.size(); i++) {
			if (hierarchy_in_cropped_img[i][3] != -1) continue;

			found_next_contour = true;
			contour = contours_in_copped_img[i];

			// obtain all the holes inside this contour
			holes.clear();
			int hole_id = hierarchy_in_cropped_img[i][2];
			while (hole_id != -1) {
				holes.push_back(contours_in_copped_img[hole_id]);
				hole_id = hierarchy_in_cropped_img[hole_id][0];
			}

			// offset back the contour and holes
			for (int i = 0; i < contour.size(); i++) {
				contour[i].x += min_x;
				contour[i].y += min_y;
			}
			for (int i = 0; i < holes.size(); i++) {
				for (int j = 0; j < holes[i].size(); j++) {
					holes[i][j].x += min_x;
					holes[i][j].y += min_y;
				}
			}

			calculateBuildings(contour, holes, next_height, buildings);
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
	int min_x = std::numeric_limits<int>::max();
	int max_x = -std::numeric_limits<int>::max();
	int min_y = std::numeric_limits<int>::max();
	int max_y = -std::numeric_limits<int>::max();
	for (int i = 0; i < contour.size(); i++) {
		min_x = std::min(min_x, contour[i].x);
		max_x = std::max(max_x, contour[i].x);
		min_y = std::min(min_y, contour[i].y);
		max_y = std::max(max_y, contour[i].y);
	}

	// create image of the contour of the current slice
	cv::Mat img(voxel_data[0].rows, voxel_data[0].cols, CV_8U, cv::Scalar(0));
	std::vector<std::vector<cv::Point>> contours(1);
	contours[0] = contour;
	cv::fillPoly(img, contours, cv::Scalar(255), cv::LINE_4);
	for (int i = 0; i < holes.size(); i++) {
		std::vector<std::vector<cv::Point>> hole_pts(1);
		hole_pts[0] = holes[i];
		cv::fillPoly(img, hole_pts, cv::Scalar(128), cv::LINE_4);
	}

	for (int i = height + 1; i < voxel_data.size(); i++) {
		int union_cnt = 0;
		int inter_cnt = 0;

		for (int r = min_y; r <= max_y; r++) {
			for (int c = min_x; c <= max_x; c++) {
				// ignore the pixels within the holes
				if (img.at<uchar>(r, c) == 128) continue;

				if (img.at<uchar>(r, c) == 255 || voxel_data[i].at<uchar>(r, c) == 255) union_cnt++;
				if (img.at<uchar>(r, c) == 255 && voxel_data[i].at<uchar>(r, c) == 255) inter_cnt++;
			}
		}

		if ((float)inter_cnt / union_cnt < threshold) {
			return i;
		}
	}

	return voxel_data.size();
}

