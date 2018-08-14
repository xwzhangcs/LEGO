#include "OrientationEstimator.h"
#include <iostream>
#include <fstream>

/**
 * Estimate the orientation of the polygons using Hough Transform.
 */
float OrientationEstimator::estimate(const std::vector<util::Polygon>& polygons) {
	int max_rho = 0;
	for (auto& polygon : polygons) {
		for (auto& pt : polygon.contour.points) {
			max_rho = std::max(max_rho, (int)std::ceil(pt.x * pt.x + pt.y * pt.y));
		}
	}

	cv::Mat_<float> HT(max_rho * 2 + 1, 180, 0.0f);
	for (auto& polygon : polygons) {
		for (auto& pt : polygon.contour.points) {
			for (int angle = 0; angle < 180; angle++) {
				float rho = pt.x * std::cos((float)angle / 180.0f * CV_PI) + pt.y * std::sin((float)angle / 180.0f * CV_PI);
				HT(std::round(rho) + max_rho, angle)++;
			}
		}
	}

	HT = HT.mul(HT);
	cv::Mat sumHT;
	cv::reduce(HT, sumHT, 0, cv::REDUCE_SUM);

	/*
	std::ofstream out("result.txt");
	for (int c = 0; c < sumHT.cols; c++) {
	out << sumHT.at<float>(0, c) << std::endl;
	}
	out.close();
	*/

	float max_votes = 0;
	int max_angle;
	for (int c = 0; c < sumHT.cols; c++) {
		if (sumHT.at<float>(0, c) > max_votes) {
			max_votes = sumHT.at<float>(0, c);
			max_angle = c;
		}
	}

	return max_angle / 180.0f * CV_PI;
}

/**
* Estimate the orientation of the polygon using Hough Transform.
*/
float OrientationEstimator::estimate(const std::vector<cv::Point2f>& polygon) {
	int max_rho = 0;
	for (auto& pt : polygon) {
		max_rho = std::max(max_rho, (int)std::ceil(pt.x * pt.x + pt.y * pt.y));
	}

	cv::Mat_<float> HT(max_rho * 2 + 1, 180, 0.0f);
	for (auto& pt : polygon) {
		for (int angle = 0; angle < 180; angle++) {
			float rho = pt.x * std::cos((float)angle / 180.0f * CV_PI) + pt.y * std::sin((float)angle / 180.0f * CV_PI);
			HT(std::round(rho) + max_rho, angle)++;
		}
	}

	HT = HT.mul(HT);
	cv::Mat sumHT;
	cv::reduce(HT, sumHT, 0, cv::REDUCE_SUM);

	/*
	std::ofstream out("result.txt");
	for (int c = 0; c < sumHT.cols; c++) {
		out << sumHT.at<float>(0, c) << std::endl;
	}
	out.close();
	*/

	float max_votes = 0;
	int max_angle;
	for (int c = 0; c < sumHT.cols; c++) {
		if (sumHT.at<float>(0, c) > max_votes) {
			max_votes = sumHT.at<float>(0, c);
			max_angle = c;
		}
	}

	return max_angle / 180.0f * CV_PI;
}
