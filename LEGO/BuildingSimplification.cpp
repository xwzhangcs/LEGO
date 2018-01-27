#include "BuildingSimplification.h"


BuildingSimplification::BuildingSimplification() {
}

cv::Rect BuildingSimplification::boundingBox(const std::vector<cv::Point>& polygon) {
	int min_x = std::numeric_limits<int>::max();
	int max_x = -std::numeric_limits<int>::max();
	int min_y = std::numeric_limits<int>::max();
	int max_y = -std::numeric_limits<int>::max();
	for (int i = 0; i < polygon.size(); i++) {
		min_x = std::min(min_x, polygon[i].x);
		max_x = std::max(max_x, polygon[i].x);
		min_y = std::min(min_y, polygon[i].y);
		max_y = std::max(max_y, polygon[i].y);
	}

	return cv::Rect(min_x, min_y, max_x - min_x + 1, max_y - min_y + 1);
}

/**
 * Calculate the intersection over union (IOU) inside the specified roi.
 * The images have to be CV_8U type.
 *
 */
double BuildingSimplification::calculateIOU(const cv::Mat& img1, const cv::Mat& img2, const cv::Rect& rect) {
	int union_cnt = 0;
	int inter_cnt = 0;

	for (int r = rect.y; r <= rect.y + rect.height; r++) {
		for (int c = rect.x; c <= rect.x + rect.width; c++) {
			if (img1.at<uchar>(r, c) == 255 || img2.at<uchar>(r, c) == 255) union_cnt++;
			if (img1.at<uchar>(r, c) == 255 && img2.at<uchar>(r, c) == 255) inter_cnt++;
		}
	}

	return (float)inter_cnt / union_cnt;
}