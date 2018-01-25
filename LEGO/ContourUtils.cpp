#include "ContourUtils.h"

namespace contour {
	
	void simplify(std::vector<cv::Point> contour, std::vector<cv::Point2f>& result, double eps) {
		// simplify contours
		/*std::vector<cv::Point> approx_contour;
		cv::approxPolyDP(contour, approx_contour, eps, true);

		if (approx_contour.size() < 2) {
			result.clear();
			return;
		}
		*/

		// regularize a contour
		regularizePolygon(contour, result);
	}

	/**
	* Regularize a polygon
	*
	* @param contour	input contour polygon
	* @param result	output regularized polygon
	*/
	void regularizePolygon(std::vector<cv::Point> contour, std::vector<cv::Point2f>& result) {
		result.clear();

		double resolution = 5.0;
		double area = cv::contourArea(contour);

		double min_cost = std::numeric_limits<double>::max();

		for (double angle = 0; angle < 180; angle += 10) {
			double theta = angle / 180 * CV_PI;
			for (int dx = 0; dx < resolution; dx++) {
				for (int dy = 0; dy < resolution; dy++) {
					// create a transformation matrix
					cv::Mat_<double> M = (cv::Mat_<double>(3, 3) << cos(theta) / resolution, -sin(theta) / resolution, dx / resolution, sin(theta) / resolution, cos(theta) / resolution, dy / resolution, 0, 0, 1);

					// transform the polygon
					std::vector<cv::Point> polygon(contour.size());
					for (int i = 0; i < contour.size(); i++) {
						cv::Mat_<double> p = (cv::Mat_<double>(3, 1) << contour[i].x, contour[i].y, 1);
						cv::Mat_<double> p2 = M * p;
						polygon[i] = cv::Point2f(p2(0, 0), p2(1, 0));
					}

					// calculate the bounding box
					cv::Point min_pt(INT_MAX, INT_MAX);
					cv::Point max_pt(INT_MIN, INT_MIN);
					for (int i = 0; i < contour.size(); i++) {
						min_pt.x = std::min(min_pt.x, polygon[i].x - 3);
						min_pt.y = std::min(min_pt.y, polygon[i].y - 3);
						max_pt.x = std::max(max_pt.x, polygon[i].x + 3);
						max_pt.y = std::max(max_pt.y, polygon[i].y + 3);
					}

					// offset the polygon
					for (int i = 0; i < polygon.size(); i++) {
						polygon[i].x -= min_pt.x;
						polygon[i].y -= min_pt.y;
					}

					// update the transformation matrix
					M = (cv::Mat_<double>(3, 3) << cos(theta) / resolution, -sin(theta) / resolution, dx / resolution - min_pt.x, sin(theta) / resolution, cos(theta) / resolution, dy / resolution - min_pt.y, 0, 0, 1);

					// create inverse transformation matrix
					cv::Mat_<double> invM = M.inv();
					
					cv::Mat img(max_pt.y - min_pt.y + 1, max_pt.x - min_pt.x + 1, CV_8U, cv::Scalar(0));

					// draw a polygon
					std::vector<std::vector<cv::Point>> polygons;
					polygons.push_back(polygon);
					cv::fillPoly(img, polygons, cv::Scalar(255), cv::LINE_8);

					// dilate the image
					cv::Mat_<uchar> kernel = (cv::Mat_<uchar>(3, 3) << 1, 1, 0, 1, 1, 0, 0, 0, 0);
					cv::Mat inflated;
					cv::dilate(img, img, kernel);

					//cv::imwrite("test.png", img);

					// extract a contour (my custom version)
					std::vector<cv::Point2f> new_contour;
					findContour(img, new_contour);

					double a = cv::contourArea(new_contour) * resolution * resolution;
					if (new_contour.size() >= 3 && a > 0) {
						// convert the polygon back to the original coordinates
						for (int i = 0; i < new_contour.size(); i++) {
							cv::Mat_<double> p = (cv::Mat_<double>(3, 1) << (double)new_contour[i].x, (double)new_contour[i].y, 1.0);
							cv::Mat_<double> p2 = invM * p;
							new_contour[i] = cv::Point2f(p2(0, 0), p2(1, 0));
						}

						// calculate cost
						double cost = 0.0;
						for (int i = 0; i < new_contour.size(); i++) {
							double min_dist = std::numeric_limits<double>::max();
							for (int j = 0; j < contour.size(); j++) {
								double dist = std::hypot(new_contour[i].x - contour[j].x, new_contour[i].y - contour[j].y);
								min_dist = std::min(min_dist, dist);
							}
							cost += min_dist;
						}
						cost /= new_contour.size();

						// calculate the cost
						cost += new_contour.size() * 0.3;

						if (cost < min_cost) {
							min_cost = cost;

							result = new_contour;
						}
					}
				}
			}
		}
	}

	/**
	* My custom contour extraction
	* Currently, only the right-angle corner is supported.
	*
	* @param img		input single-channel image (0 - background, 255 - footprint)
	* @param contour	output contour polygon
	*/
	void findContour(const cv::Mat& img, std::vector<cv::Point2f>& contour) {
		contour.clear();

		// find the start point
		cv::Point2f start;
		bool found = false;
		for (int r = 0; r < img.rows && !found; r++) {
			for (int c = 0; c < img.cols; c++) {
				if (img.at<uchar>(r, c) == 255) {
					start = cv::Point(c, r);
					found = true;
					break;
				}
			}
		}

		cv::Point2f prev_dir(1, 0);
		cv::Point2f cur = start;
		contour.push_back(cur);
		int cnt = 0;
		do {
			cnt++;
			if (cnt > 10000) {
				break;
			}

			cv::Point2f left_dir(prev_dir.y, -prev_dir.x);
			cv::Point2f next = cur + left_dir;
			if (img.at<uchar>(next.y, next.x) == 255) {
				if (contour.size() > 0 && contour.back() != cur) contour.push_back(cur);
				cur = next;
				prev_dir = left_dir;
				continue;
			}

			/*
			cv::Point left_fore_dir = prev_dir + left_dir;
			if (std::abs(left_fore_dir.x) > 1) left_fore_dir.x /= std::abs(left_fore_dir.x);
			if (std::abs(left_fore_dir.y) > 1) left_fore_dir.y /= std::abs(left_fore_dir.y);
			next = cur + left_fore_dir;
			if (img.at<uchar>(next.y, next.x) == 255) {
				if (contour.size() > 0 && contour.back() != cur) contour.push_back(cur);
				cur = next;
				prev_dir = left_fore_dir;
				continue;
			}
			*/

			next = cur + prev_dir;
			if (img.at<uchar>(next.y, next.x) == 255) {
				cur = next;
				continue;
			}

			/*
			cv::Point right_fore_dir = prev_dir - left_dir;
			if (std::abs(right_fore_dir.x) > 1) right_fore_dir.x /= std::abs(right_fore_dir.x);
			if (std::abs(right_fore_dir.y) > 1) right_fore_dir.y /= std::abs(right_fore_dir.y);
			next = cur + right_fore_dir;
			if (img.at<uchar>(next.y, next.x) == 255) {
				if (contour.size() > 0 && contour.back() != cur) contour.push_back(cur);
				cur = next;
				prev_dir = right_fore_dir;
				continue;
			}
			*/

			cv::Point2f right_dir(-prev_dir.y, prev_dir.x);
			next = cur + right_dir;
			if (img.at<uchar>(next.y, next.x) == 255) {
				if (contour.size() > 0 && contour.back() != cur) contour.push_back(cur);
				cur = next;
				prev_dir = right_dir;
				continue;
			}

			cv::Point2f back_dir = -prev_dir;
			next = cur + back_dir;
			if (img.at<uchar>(next.y, next.x) == 255) {
				//contour.push_back(cur);
				cur = next;
				prev_dir = back_dir;
				continue;
			}

			break;
		} while (cur != start);
	}

}
