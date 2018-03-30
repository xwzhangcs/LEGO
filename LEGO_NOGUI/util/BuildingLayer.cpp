#include "BuildingLayer.h"

namespace util {

	BuildingLayer::BuildingLayer(int building_id, float bottom_height, float top_height) {
		this->building_id = building_id;
		this->bottom_height = bottom_height;
		this->top_height = top_height;
	}

	BuildingLayer::BuildingLayer(int building_id, const std::vector<util::Polygon>& footprints, float bottom_height, float top_height) {
		this->building_id = building_id;
		this->footprints = footprints;
		this->bottom_height = bottom_height;
		this->top_height = top_height;
	}

	std::vector<util::Polygon> BuildingLayer::selectRepresentativeContours() const {
		// calculate the bounding box
		int min_x = INT_MAX;
		int min_y = INT_MAX;
		int max_x = INT_MIN;
		int max_y = INT_MIN;
		for (auto& polygons : raw_footprints) {
			for (auto& polygon : polygons) {
				Ring contour = polygon.contour.getActualPoints();
				for (auto& pt : contour) {
					min_x = std::min(min_x, (int)pt.x);
					min_y = std::min(min_y, (int)pt.y);
					max_x = std::max(max_x, (int)(pt.x + 0.5));
					max_y = std::max(max_y, (int)(pt.y + 0.5));
				}
			}
		}

		// calculate the average contour image
		cv::Mat_<double> total_img = cv::Mat_<double>::zeros(max_y - min_y + 1, max_x - min_x + 1);
		for (auto& polygons : raw_footprints) {
			cv::Mat_<uchar> img = cv::Mat_<uchar>::zeros(max_y - min_y + 1, max_x - min_x + 1);
			for (auto& polygon : polygons) {
				std::vector<std::vector<cv::Point>> contour_points(1 + polygon.holes.size());
				Ring contour = polygon.contour.getActualPoints();
				contour_points[0].resize(contour.size());
				for (int i = 0; i < contour.size(); i++) {
					contour_points[0][i] = cv::Point(contour[i].x - min_x, contour[i].y - min_y);
				}
				for (int i = 0; i < polygon.holes.size(); i++) {
					Ring hole = polygon.holes[i].getActualPoints();
					contour_points[i + 1].resize(hole.size());
					for (int j = 0; j < hole.size(); j++) {
						contour_points[i + 1][j] = cv::Point(hole[j].x - min_x, hole[j].y - min_y);
					}
				}
				cv::fillPoly(img, contour_points, cv::Scalar(255), cv::LINE_4);
			}

			cv::Mat_<double> img_double;
			img.convertTo(img_double, CV_64F);
			total_img += img_double;
		}
		total_img /= raw_footprints.size();
		cv::Mat_<uchar> mean_img;
		total_img.convertTo(mean_img, CV_8U);

		// find the most representative slice
		double best_iou = 0;
		int best_slice = -1;
		for (int i = 0; i < raw_footprints.size(); i++) {
			cv::Mat_<uchar> img = cv::Mat_<uchar>::zeros(max_y - min_y + 1, max_x - min_x + 1);
			for (auto& polygon : raw_footprints[i]) {
				std::vector<std::vector<cv::Point>> contour_points(1 + polygon.holes.size());
				Ring contour = polygon.contour.getActualPoints();
				contour_points[0].resize(contour.size());
				for (int i = 0; i < contour.size(); i++) {
					contour_points[0][i] = cv::Point(contour[i].x - min_x, contour[i].y - min_y);
				}
				for (int i = 0; i < polygon.holes.size(); i++) {
					Ring hole = polygon.holes[i].getActualPoints();
					contour_points[i + 1].resize(hole.size());
					for (int j = 0; j < hole.size(); j++) {
						contour_points[i + 1][j] = cv::Point(hole[j].x - min_x, hole[j].y - min_y);
					}
				}
				cv::fillPoly(img, contour_points, cv::Scalar(255), cv::LINE_4);
			}

			double iou = util::calculateIOU(img, mean_img);
			if (iou > best_iou) {
				best_iou = iou;
				best_slice = i;
			}
		}

		return raw_footprints[best_slice];
	}

}