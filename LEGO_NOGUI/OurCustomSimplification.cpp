#include "OurCustomSimplification.h"
#include "ContourUtils.h"

namespace simp {

	/**
	* Simplify the footprint of the layer.
	*
	* @param slices	slice images of the layer
	* @param epsilon	simplification parameter
	* @return			simplified footprint
	*/

	util::Polygon OurCustomSimplification::simplify(const cv::Mat& slice, int resolution, float& angle, int& dx, int& dy) {
		// make sure there is a building in the layer
		std::vector<util::Polygon> polygons = util::findContours(slice);
		if (polygons.size() == 0) throw "No building is found.";

		util::Polygon ans;

		if (angle == -1) {
			std::tuple<float, int, int> best_mat = simplifyContour(polygons[0].contour, ans.contour, resolution);
			angle = std::get<0>(best_mat);
			dx = std::get<1>(best_mat);
			dy = std::get<2>(best_mat);
		}
		else {
			simplifyContour(polygons[0].contour, ans.contour, resolution, angle, dx, dy);
		}
		if (ans.contour.size() < 3) throw "Invalid contour. #vertices is less than 3.";

		// simplify the hole as well
		for (int i = 0; i < polygons[0].holes.size(); i++) {
			std::vector<cv::Point2f> simplified_hole;
			simplifyContour(polygons[0].holes[i], simplified_hole, resolution);
			if (simplified_hole.size() >= 3) {
				ans.holes.push_back(simplified_hole);
			}
		}

		// ToDo:
		// Should we check if the holes are inside the contour?

		return ans;
	}

	/**
	* Simplify and regularize a polygon
	*
	* @param contour		input contour polygon
	* @param result		output regularized polygon
	* @param resolution	resolution which defines how much simplified
	* @return				best angle, dx, and dy that yiled the resulting simplified polygon
	*/
	std::tuple<float, int, int> OurCustomSimplification::simplifyContour(const std::vector<cv::Point2f>& contour, std::vector<cv::Point2f>& result, int resolution) {
		result.clear();

		double min_cost = std::numeric_limits<double>::max();
		float best_angle;
		int best_dx;
		int best_dy;

		for (float angle = 0; angle < 90; angle += 10) {
			//std::cout << "angle=" << angle << std::endl;

			for (int dx = 0; dx < resolution; dx++) {
				for (int dy = 0; dy < resolution; dy++) {
					std::vector<cv::Point2f> simplified_contour;
					try {
						double cost = simplifyContour(contour, simplified_contour, resolution, angle, dx, dy);

						if (cost < min_cost) {
							min_cost = cost;
							best_angle = angle;
							best_dx = dx;
							best_dy = dy;

							result = simplified_contour;
						}
					}
					catch (...) {
					}
				}
			}
		}

		if (min_cost == std::numeric_limits<double>::max()) throw "No simplified polygon was found.";

		return std::make_tuple(best_angle, best_dx, best_dy);
	}

	/**
	* Simplify and regularize a polygon using the specified angle, dx, and dy.
	*
	* @param contour		input contour polygon
	* @param result			output regularized polygon
	* @param resolution		resolution which defines how much simplified
	* @return				best cost
	*/
	double OurCustomSimplification::simplifyContour(const std::vector<cv::Point2f>& contour, std::vector<cv::Point2f>& result, int resolution, float angle, int dx, int dy) {
		double theta = angle / 180 * CV_PI;

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
		
		// calculate the bounding box
		cv::Point min_pt(INT_MAX, INT_MAX);
		cv::Point max_pt(INT_MIN, INT_MIN);
		for (int i = 0; i < small_aa_polygon.size(); i++) {
			min_pt.x = std::min(min_pt.x, small_aa_polygon[i].x - 3);
			min_pt.y = std::min(min_pt.y, small_aa_polygon[i].y - 3);
			max_pt.x = std::max(max_pt.x, small_aa_polygon[i].x + 3);
			max_pt.y = std::max(max_pt.y, small_aa_polygon[i].y + 3);
		}

		cv::Mat img(max_pt.y - min_pt.y + 1, max_pt.x - min_pt.x + 1, CV_8U, cv::Scalar(0));

		// draw a polygon
		std::vector<std::vector<cv::Point>> polygons;
		polygons.push_back(small_aa_polygon);
		cv::fillPoly(img, polygons, cv::Scalar(255), cv::LINE_4, 0, cv::Point(-min_pt.x, -min_pt.y));

		// dilate the image
		cv::Mat_<uchar> kernel = (cv::Mat_<uchar>(3, 3) << 1, 1, 0, 1, 1, 0, 0, 0, 0);
		cv::Mat inflated;
		cv::dilate(img, img, kernel);

		// extract a contour (my custom version)
		std::vector<cv::Point> simplified_small_aa_contour;
		util::findContour(img, simplified_small_aa_contour);
		if (simplified_small_aa_contour.size() < 3) throw "Invalid contour. #vertices is less than 3.";

		// offset back and scale up the simplified scale-down polygon
		std::vector<cv::Point> simplified_aa_contour(simplified_small_aa_contour.size());
		for (int i = 0; i < simplified_small_aa_contour.size(); i++) {
			simplified_aa_contour[i] = cv::Point((simplified_small_aa_contour[i].x + min_pt.x) * resolution, (simplified_small_aa_contour[i].y + min_pt.y) * resolution);
		}

		// refine the simplified contour
		double cost = 1.0 / (0.01 + optimizeSimplifiedContour(aa_contour_int, simplified_aa_contour));

		// check if the simplified contour has self-intersecting
		simplified_aa_contour = util::removeRedundantPoint(simplified_aa_contour);
		for (int i = 0; i < simplified_aa_contour.size(); i++) {
			for (int j = i + 1; j < simplified_aa_contour.size(); j++) {
				if (simplified_aa_contour[i] == simplified_aa_contour[j]) {
					result.clear();
					return std::numeric_limits<double>::max();
				}
			}
		}

		// calculate the cost
		cost += simplified_aa_contour.size() * 0.2;

		// transform back the simplified contour
		std::vector<cv::Point2f> simplified_contour(simplified_aa_contour.size());
		for (int i = 0; i < simplified_aa_contour.size(); i++) {
			cv::Mat_<double> p = (cv::Mat_<double>(3, 1) << (double)simplified_aa_contour[i].x, (double)simplified_aa_contour[i].y, 1.0);
			cv::Mat_<double> p2 = invM * p;
			simplified_contour[i] = cv::Point2f(p2(0, 0), p2(1, 0));
		}

		result = simplified_contour;

		return cost;
	}

	/**
	* Optimize the simplified contour such that it best fit to the input contour.
	* Return the IOU of the input contour and the optimized simplified contour.
	*
	* @param contour				input contour
	* @param simplified_contour	simplified contour
	* @return						intersection over union (IOU)
	*/
	double OurCustomSimplification::optimizeSimplifiedContour(const std::vector<cv::Point>& contour, std::vector<cv::Point>& simplified_contour) {
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
		util::createImageFromContour(max_x - min_x + 1, max_y - min_y + 1, contour, cv::Point(-min_x, -min_y), img);

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
				if ((next_it != x_map.end() && prop_x_map[it->first] <= prop_x_map[next_it->first]) || (next_it == x_map.end() && prop_x_map[it->first] <= max_x)) {
					std::vector<cv::Point> proposed_contour = proposedContour(simplified_contour, prop_x_map, y_map);
					cv::Mat img2;
					util::createImageFromContour(max_x - min_x + 1, max_y - min_y + 1, proposed_contour, cv::Point(-min_x, -min_y), img2);
					double score = util::calculateIOU(img, img2);
					if (score > best_score) {
						best_score = score;
						best_x_map = prop_x_map;
						best_y_map = y_map;
						updated = true;
					}
				}

				prop_x_map = x_map;
				prop_x_map[it->first]--;
				if ((prev_it != x_map.end() && prop_x_map[it->first] >= prop_x_map[prev_it->first]) || (prev_it == x_map.end() && prop_x_map[it->first] >= min_x)) {
					std::vector<cv::Point> proposed_contour = proposedContour(simplified_contour, prop_x_map, y_map);
					cv::Mat img2;
					util::createImageFromContour(max_x - min_x + 1, max_y - min_y + 1, proposed_contour, cv::Point(-min_x, -min_y), img2);
					double score = util::calculateIOU(img, img2);
					if (score > best_score) {
						best_score = score;
						best_x_map = prop_x_map;
						best_y_map = y_map;
						updated = true;
					}
				}

				prev_it = it;
				if (next_it != x_map.end())	next_it++;
			}

			prev_it = y_map.end();
			next_it = y_map.begin();
			next_it++;
			for (auto it = y_map.begin(); it != y_map.end(); it++) {
				std::map<int, int> prop_y_map = y_map;
				prop_y_map[it->first]++;
				if ((next_it != y_map.end() && prop_y_map[it->first] <= prop_y_map[next_it->first]) || (next_it == y_map.end() && prop_y_map[it->first] <= max_y)) {
					std::vector<cv::Point> proposed_contour = proposedContour(simplified_contour, x_map, prop_y_map);
					cv::Mat img2;
					util::createImageFromContour(max_x - min_x + 1, max_y - min_y + 1, proposed_contour, cv::Point(-min_x, -min_y), img2);
					double score = util::calculateIOU(img, img2);
					if (score > best_score) {
						best_score = score;
						best_x_map = x_map;
						best_y_map = prop_y_map;
						updated = true;
					}
				}

				prop_y_map = y_map;
				prop_y_map[it->first]--;
				if ((prev_it != y_map.end() && prop_y_map[it->first] >= prop_y_map[prev_it->first]) || (prev_it == y_map.end() && prop_y_map[it->first] >= min_y)) {
					std::vector<cv::Point> proposed_contour = proposedContour(simplified_contour, x_map, prop_y_map);
					cv::Mat img2;
					util::createImageFromContour(max_x - min_x + 1, max_y - min_y + 1, proposed_contour, cv::Point(-min_x, -min_y), img2);
					double score = util::calculateIOU(img, img2);
					if (score > best_score) {
						best_score = score;
						best_x_map = x_map;
						best_y_map = prop_y_map;
						updated = true;
					}
				}

				prev_it = it;
				if (next_it != y_map.end())	next_it++;
			}

			// if no update, stop the optimization
			if (!updated) break;

			x_map = best_x_map;
			y_map = best_y_map;
		}

		simplified_contour = proposedContour(simplified_contour, x_map, y_map);
		return best_score;
	}

	std::vector<cv::Point> OurCustomSimplification::proposedContour(const std::vector<cv::Point>& contour, std::map<int, int>& x_map, std::map<int, int>& y_map) {
		std::vector<cv::Point> prop_contour(contour.size());
		for (int i = 0; i < contour.size(); i++) {
			prop_contour[i] = cv::Point(x_map[contour[i].x], y_map[contour[i].y]);
		}
		return prop_contour;
	}

}