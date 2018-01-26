#include "ContourUtils.h"
#include <iostream>

namespace contour {
	
	/**
 	 * Simplify and regularize a polygon
	 *
	 * @param contour	input contour polygon
	 * @param result		output regularized polygon
	 */
	cv::Mat_<double> simplify(std::vector<cv::Point> contour, std::vector<cv::Point2f>& result, double resolution) {
		result.clear();

		double area = cv::contourArea(contour);

		double min_cost = std::numeric_limits<double>::max();
		cv::Mat_<double> best_M;

		for (double angle = 0; angle < 90; angle += 20) {
			//std::cout << "angle=" << angle << std::endl;

			double theta = angle / 180 * CV_PI;
			for (int dx = 0; dx < resolution; dx++) {
				for (int dy = 0; dy < resolution; dy++) {
					// create a transformation matrix
					cv::Mat_<double> M = (cv::Mat_<double>(3, 3) << cos(theta), -sin(theta), dx, sin(theta), cos(theta), dy, 0, 0, 1);

					// create inverse transformation matrix
					cv::Mat_<double> invM = M.inv();

					// transform the polygon
					std::vector<cv::Point2f> aa_contour(contour.size());
					for (int i = 0; i < contour.size(); i++) {
						cv::Mat_<double> p = (cv::Mat_<double>(3, 1) << contour[i].x, contour[i].y, 1);
						cv::Mat_<double> p2 = M * p;
						aa_contour[i] = cv::Point2f(p2(0, 0), p2(1, 0));
					}

					// create the integer coordinates of the polygon
					std::vector<cv::Point> aa_contour_int(aa_contour.size());
					for (int i = 0; i < aa_contour.size(); i++) {
						aa_contour_int[i] = cv::Point(aa_contour[i]);
					}

					// scale down the polygon based on the resolution
					std::vector<cv::Point> small_aa_polygon(aa_contour.size());
					for (int i = 0; i < aa_contour.size(); i++) {
						small_aa_polygon[i] = cv::Point(aa_contour[i].x / resolution, aa_contour[i].y / resolution);
					}
					
					// calculate the bounding box of the scale-down polygon
					cv::Point min_pt(INT_MAX, INT_MAX);
					cv::Point max_pt(INT_MIN, INT_MIN);
					for (int i = 0; i < small_aa_polygon.size(); i++) {
						min_pt.x = std::min(min_pt.x, small_aa_polygon[i].x - 3);
						min_pt.y = std::min(min_pt.y, small_aa_polygon[i].y - 3);
						max_pt.x = std::max(max_pt.x, small_aa_polygon[i].x + 3);
						max_pt.y = std::max(max_pt.y, small_aa_polygon[i].y + 3);
					}

					// draw a polygon
					cv::Mat small_aa_polygon_img(max_pt.y - min_pt.y + 1, max_pt.x - min_pt.x + 1, CV_8U, cv::Scalar(0));
					std::vector<std::vector<cv::Point>> small_aa_polygons;
					small_aa_polygons.push_back(small_aa_polygon);
					cv::fillPoly(small_aa_polygon_img, small_aa_polygons, cv::Scalar(255), cv::LINE_4, 0, -min_pt);

					// dilate the image
					cv::Mat_<uchar> kernel = (cv::Mat_<uchar>(3, 3) << 1, 1, 0, 1, 1, 0, 0, 0, 0);
					cv::Mat inflated;
					cv::dilate(small_aa_polygon_img, small_aa_polygon_img, kernel);

					//cv::imwrite("test.png", small_polygon_img);

					// extract a contour (my custom version)
					std::vector<cv::Point> simplified_small_aa_contour;
					findContour(small_aa_polygon_img, simplified_small_aa_contour);

					double a = cv::contourArea(simplified_small_aa_contour) * resolution * resolution;
					if (simplified_small_aa_contour.size() >= 3 && a > 0) {
						// offset back and scale up the simplified scale-down polygon
						std::vector<cv::Point> simplified_aa_contour(simplified_small_aa_contour.size());
						for (int i = 0; i < simplified_small_aa_contour.size(); i++) {
							simplified_aa_contour[i] = cv::Point((simplified_small_aa_contour[i].x + min_pt.x) * resolution, (simplified_small_aa_contour[i].y + min_pt.y) * resolution);
						}
						
						// refine the simplified contour
						std::vector<cv::Point> simplified_aa_contour2 = simplified_aa_contour;
						double cost = 1.0 / (0.01 + optimizeSimplifiedContour(aa_contour_int, simplified_aa_contour));

						// calculate the cost
						cost += simplified_aa_contour.size() * 0.2;

						if (cost < min_cost) {
							min_cost = cost;
							best_M = M;

							// transform back the simplified contour
							std::vector<cv::Point2f> simplified_contour(simplified_aa_contour.size());
							for (int i = 0; i < simplified_aa_contour.size(); i++) {
								cv::Mat_<double> p = (cv::Mat_<double>(3, 1) << (double)simplified_aa_contour[i].x, (double)simplified_aa_contour[i].y, 1.0);
								cv::Mat_<double> p2 = invM * p;
								simplified_contour[i] = cv::Point2f(p2(0, 0), p2(1, 0));
							}

							result = simplified_contour;
						}
					}
				}
			}
		}

		return best_M.clone();
	}

	/**
	* My custom contour extraction
	* Currently, only the right-angle corner is supported.
	*
	* @param img		input single-channel image (0 - background, 255 - footprint)
	* @param contour	output contour polygon
	*/
	void findContour(const cv::Mat& img, std::vector<cv::Point>& contour) {
		contour.clear();

		// find the start point
		cv::Point start;
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

		cv::Point prev_dir(1, 0);
		cv::Point cur = start;
		contour.push_back(cur);
		int cnt = 0;
		do {
			cnt++;
			if (cnt > 1000) {
				break;
			}

			cv::Point left_dir(prev_dir.y, -prev_dir.x);
			cv::Point next = cur + left_dir;
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

			cv::Point right_dir(-prev_dir.y, prev_dir.x);
			next = cur + right_dir;
			if (img.at<uchar>(next.y, next.x) == 255) {
				if (contour.size() > 0 && contour.back() != cur) contour.push_back(cur);
				cur = next;
				prev_dir = right_dir;
				continue;
			}

			cv::Point back_dir = -prev_dir;
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

	/**
	 * Optimize the simplified contour such that it best fit to the input contour.
	 * Return the IOU of the input contour and the optimized simplified contour.
	 *
	 * @param contour				input contour
	 * @param simplified_contour	simplified contour
	 * @return						intersection over union (IOU)
	 */
	double optimizeSimplifiedContour(const std::vector<cv::Point>& contour, std::vector<cv::Point>& simplified_contour) {
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
		for (int i = 0; i < simplified_contour.size(); i++) {
			min_x = std::min(min_x, simplified_contour[i].x);
			max_x = std::max(max_x, simplified_contour[i].x);
			min_y = std::min(min_y, simplified_contour[i].y);
			max_y = std::max(max_y, simplified_contour[i].y);
		}

		// create the image of the input contour
		cv::Mat img;
		createImageFromContour(max_x - min_x + 1, max_y - min_y + 1, contour, min_x, min_y, img);
		cv::imwrite("test.png", img);

		// list up the parameters
		std::map<int, int> x_map;
		std::map<int, int> y_map;
		for (int i = 0; i < simplified_contour.size(); i++) {
			if (x_map.find(simplified_contour[i].x) == x_map.end()) {
				x_map[simplified_contour[i].x] = simplified_contour[i].x;
			}
			if (y_map.find(simplified_contour[i].y) == y_map.end()) {
				y_map[simplified_contour[i].y] = simplified_contour[i].y;
			}
		}

		// optimize the parameters
		double best_score = 0;
		std::map<int, int> best_x_map;
		std::map<int, int> best_y_map;

		int cnt = 0;
		for (int iter = 0; iter < 30; iter++) {
			bool updated = false;

			auto prev_it = x_map.end();
			auto next_it = x_map.begin();
			next_it++;
			for (auto it = x_map.begin(); it != x_map.end(); it++) {
				std::map<int, int> prop_x_map = x_map;
				prop_x_map[it->first]++;
				if ((next_it != x_map.end() && prop_x_map[it->first] < prop_x_map[next_it->first]) || (next_it == x_map.end() && prop_x_map[it->first] <= max_x)) {
					std::vector<cv::Point> proposed_contour = proposedContour(simplified_contour, prop_x_map, y_map);
					cv::Mat img2;
					createImageFromContour(max_x - min_x + 1, max_y - min_y + 1, proposed_contour, min_x, min_y, img2);
					double score = calculateIOU(img, img2);
					if (score > best_score) {
						best_score = score;
						best_x_map = prop_x_map;
						best_y_map = y_map;
						updated = true;
					}
				}

				prop_x_map = x_map;
				prop_x_map[it->first]--;
				if ((prev_it != x_map.end() && prop_x_map[it->first] > prop_x_map[prev_it->first]) || (prev_it == x_map.end() && prop_x_map[it->first] >= 0)) {
					std::vector<cv::Point> proposed_contour = proposedContour(simplified_contour, prop_x_map, y_map);
					cv::Mat img2;
					createImageFromContour(max_x - min_x + 1, max_y - min_y + 1, proposed_contour, min_x, min_y, img2);
					double score = calculateIOU(img, img2);
					if (score > best_score) {
						best_score = score;
						best_x_map = prop_x_map;
						best_y_map = y_map;
						updated = true;
					}
				}

				prev_it = it;
				next_it++;
			}

			prev_it = y_map.end();
			next_it = y_map.begin();
			for (auto it = y_map.begin(); it != y_map.end(); it++) {
				std::map<int, int> prop_y_map = y_map;
				prop_y_map[it->first]++;
				if ((next_it != y_map.end() && prop_y_map[it->first] < prop_y_map[next_it->first]) || (next_it == y_map.end() && prop_y_map[it->first] <= max_y)) {
					std::vector<cv::Point> proposed_contour = proposedContour(simplified_contour, x_map, prop_y_map);
					cv::Mat img2;
					createImageFromContour(max_x - min_x + 1, max_y - min_y + 1, proposed_contour, min_x, min_y, img2);
					double score = calculateIOU(img, img2);
					if (score > best_score) {
						best_score = score;
						best_x_map = x_map;
						best_y_map = prop_y_map;
						updated = true;
					}
				}

				prop_y_map = y_map;
				prop_y_map[it->first]--;
				if ((prev_it != y_map.end() && prop_y_map[it->first] > prop_y_map[prev_it->first]) || (prev_it == y_map.end() && prop_y_map[it->first] >= 0)) {
					std::vector<cv::Point> proposed_contour = proposedContour(simplified_contour, x_map, prop_y_map);
					cv::Mat img2;
					createImageFromContour(max_x - min_x + 1, max_y - min_y + 1, proposed_contour, min_x, min_y, img2);
					double score = calculateIOU(img, img2);
					if (score > best_score) {
						best_score = score;
						best_x_map = x_map;
						best_y_map = prop_y_map;
						updated = true;
					}
				}
			}

			// if no update, stop the optimization
			if (!updated) break;

			x_map = best_x_map;
			y_map = best_y_map;
		}

		simplified_contour = proposedContour(simplified_contour, x_map, y_map);
		return best_score;
	}

	void createImageFromContour(int width, int height, const std::vector<cv::Point>& contour, int min_x, int min_y, cv::Mat& result) {
		result = cv::Mat(height, width, CV_8U, cv::Scalar(0));
		std::vector<std::vector<cv::Point>> contour_points(1);
		contour_points[0].resize(contour.size());
		for (int i = 0; i < contour.size(); i++) {
			contour_points[0].push_back(cv::Point(contour[i].x - min_x, contour[i].y - min_y));
		}
		cv::fillPoly(result, contour_points, cv::Scalar(255));
	}

	double calculateIOU(const cv::Mat& img, const cv::Mat& img2) {
		int union_cnt = 0;
		int inter_cnt = 0;
		for (int r = 0; r < img.rows; r++) {
			for (int c = 0; c < img.cols; c++) {
				if (img.at<uchar>(r, c) == 255 || img2.at<uchar>(r, c) == 255) union_cnt++;
				if (img.at<uchar>(r, c) == 255 && img2.at<uchar>(r, c) == 255) inter_cnt++;
			}
		}

		return (double)inter_cnt / union_cnt;
	}

	std::vector<cv::Point> proposedContour(const std::vector<cv::Point>& contour, std::map<int, int>& x_map, std::map<int, int>& y_map) {
		std::vector<cv::Point> prop_contour(contour.size());
		for (int i = 0; i < contour.size(); i++) {
			prop_contour[i] = cv::Point(x_map[contour[i].x], y_map[contour[i].y]);
		}
		return prop_contour;
	}

}
