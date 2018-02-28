#include "RightAngleSimplification.h"
#include "../util/ContourUtils.h"

namespace simp {

	/**
	* Simplify the footprint of the layer.
	*
	* @param slices	slice images of the layer
	* @param epsilon	simplification parameter
	* @return			simplified footprint
	*/
	util::Polygon RightAngleSimplification::simplify(const util::Polygon& polygon, int resolution, float& angle, int& dx, int& dy) {
		util::Polygon ans;

		if (angle == -1) {
			std::tuple<float, int, int> best_mat = simplifyContour(polygon.contour, ans.contour, resolution);
			angle = std::get<0>(best_mat);
			dx = std::get<1>(best_mat);
			dy = std::get<2>(best_mat);
		}
		else {
			simplifyContour(polygon.contour, ans.contour, resolution, angle, dx, dy);
		}
		if (ans.contour.size() < 3) throw "Invalid contour. #vertices is less than 3.";

		// simplify the hole as well
		for (int i = 0; i < polygon.holes.size(); i++) {
			try {
				util::Ring simplified_hole;
				simplifyContour(polygon.holes[i], simplified_hole, resolution, angle, dx, dy);
				if (simplified_hole.size() >= 3) {
					ans.holes.push_back(simplified_hole);
				}
			}
			catch (...) {
			}
		}

		// The transfomration matrix should be same for the external contour and the internal holes
		ans.mat = ans.contour.mat;

		// ToDo:
		// Should we check if the holes are inside the contour?

		decomposePolygon(ans);

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
	std::tuple<float, int, int> RightAngleSimplification::simplifyContour(const util::Ring& contour, util::Ring& result, int resolution) {
		result.clear();

		double min_cost = std::numeric_limits<double>::max();
		float best_angle;
		int best_dx;
		int best_dy;

		for (float angle = 0; angle < 90; angle += 10) {
			//std::cout << "angle=" << angle << std::endl;

			for (int dx = 0; dx < resolution; dx++) {
				for (int dy = 0; dy < resolution; dy++) {
					util::Ring simplified_contour;
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
	double RightAngleSimplification::simplifyContour(const util::Ring& contour, util::Ring& result, int resolution, float angle, int dx, int dy) {
		double theta = angle / 180 * CV_PI;

		// create a transformation matrix
		cv::Mat_<float> M = (cv::Mat_<float>(3, 3) << cos(theta), -sin(theta), dx, sin(theta), cos(theta), dy, 0, 0, 1);

		// create inverse transformation matrix
		cv::Mat_<float> invM = M.inv();

		// transform the polygon
		std::vector<cv::Point2f> aa_contour(contour.size());
		for (int i = 0; i < contour.size(); i++) {
			cv::Mat_<float> p = (cv::Mat_<float>(3, 1) << contour[i].x, contour[i].y, 1);
			cv::Mat_<float> p2 = M * p;
			aa_contour[i] = cv::Point2f(p2(0, 0), p2(1, 0));
		}

		// create the integer coordinates of the polygon
		std::vector<cv::Point> aa_contour_int(aa_contour.size());
		for (int i = 0; i < aa_contour.size(); i++) {
			aa_contour_int[i] = cv::Point(std::round(aa_contour[i].x), std::round(aa_contour[i].y));
		}

		// scale down the polygon based on the resolution
		std::vector<cv::Point> small_aa_polygon(aa_contour.size());
		for (int i = 0; i < aa_contour.size(); i++) {
			small_aa_polygon[i] = cv::Point(std::round(aa_contour[i].x / resolution), std::round(aa_contour[i].y / resolution));
		}
		
		// calculate the bounding box
		cv::Rect bbox = util::boundingBox(small_aa_polygon);

		cv::Mat_<uchar> img;// = cv::Mat_<uchar>::zeros(bbox.height, bbox.width);
		util::createImageFromContour(bbox.width, bbox.height, small_aa_polygon, cv::Point(-bbox.x, -bbox.y), img);

		std::vector<util::Polygon> polygons = util::findContours(img, true);
		if (polygons.size() == 0) throw "No contour is found.";

		// offset back and scale up the simplified scale-down polygon
		std::vector<cv::Point> simplified_aa_contour(polygons[0].contour.size());
		for (int i = 0; i < polygons[0].contour.size(); i++) {
			simplified_aa_contour[i] = cv::Point((polygons[0].contour[i].x + bbox.x) * resolution, (polygons[0].contour[i].y + bbox.y) * resolution);
		}
		
		// refine the simplified contour
		double cost = 1.0 / (0.01 + optimizeSimplifiedContour(aa_contour_int, simplified_aa_contour));
		
		// generate a simplified contour removing self-intersections
		bbox = util::boundingBox(simplified_aa_contour);
		util::createImageFromContour(bbox.width, bbox.height, simplified_aa_contour, cv::Point(-bbox.x, -bbox.y), img);
		polygons = util::findContours(img, false);
		if (polygons.size() == 0) throw "No valid contour is generated.";
		for (int i = 0; i < polygons[0].contour.size(); i++) {
			polygons[0].contour[i] += cv::Point2f(bbox.x, bbox.y);
		}

		// calculate the cost
		cost += polygons[0].contour.size() * 0.2;

		// transform back the simplified contour
		util::Ring simplified_contour = polygons[0].contour;
		simplified_contour.mat = invM;

		result = simplified_contour;

		return cost;
	}

	/**
	 * Optimize the simplified contour such that it best fits to the input contour.
	 * Return the IOU of the input contour and the optimized simplified contour.
	 *
	 * @param contour				input contour
	 * @param simplified_contour		simplified contour
	 * @return						intersection over union (IOU)
	 */
	double RightAngleSimplification::optimizeSimplifiedContour(const std::vector<cv::Point>& contour, std::vector<cv::Point>& simplified_contour) {
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
		cv::Mat_<uchar> img;
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
					cv::Mat_<uchar> img2;
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
					cv::Mat_<uchar> img2;
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
					cv::Mat_<uchar> img2;
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
					cv::Mat_<uchar> img2;
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

	std::vector<cv::Point> RightAngleSimplification::proposedContour(const std::vector<cv::Point>& contour, std::map<int, int>& x_map, std::map<int, int>& y_map) {
		std::vector<cv::Point> prop_contour(contour.size());
		for (int i = 0; i < contour.size(); i++) {
			prop_contour[i] = cv::Point(x_map[contour[i].x], y_map[contour[i].y]);
		}
		return prop_contour;
	}

	void RightAngleSimplification::decomposePolygon(util::Polygon& polygon) {
		// list up all xy coordinates
		std::map<float, bool> x_map;
		std::map<float, bool> y_map;
		for (int i = 0; i < polygon.contour.size(); i++) {
			x_map[polygon.contour[i].x] = true;
			y_map[polygon.contour[i].y] = true;
		}
		for (int i = 0; i < polygon.holes.size(); i++) {
			for (int j = 0; j < polygon.holes[i].size(); j++) {
				x_map[polygon.holes[i][j].x] = true;
				y_map[polygon.holes[i][j].y] = true;
			}
		}

		// create vector of xy coordinates
		std::vector<float> x_coords;
		for (auto it = x_map.begin(); it != x_map.end(); it++) {
			x_coords.push_back(it->first);
		}
		std::vector<float> y_coords;
		for (auto it = y_map.begin(); it != y_map.end(); it++) {
			y_coords.push_back(it->first);
		}

		std::vector<std::vector<bool>> grid(y_coords.size() - 1, std::vector<bool>(x_coords.size() - 1, false));
		int cell_count = 0;
		for (int i = 0; i < x_coords.size() - 1; i++) {
			for (int j = 0; j < y_coords.size() - 1; j++) {
				if (util::withinPolygon(cv::Point2f((x_coords[i] + x_coords[i + 1]) * 0.5, (y_coords[j] + y_coords[j + 1]) * 0.5), polygon)) {
					grid[j][i] = true;
					cell_count++;
				}
			}
		}

		polygon.primitive_shapes.clear();

		while (cell_count > 0) {
			int x, y, width, height;
			findMaximumRectangle(grid, x_coords, y_coords, x, y, width, height);
			
			// update grid
			for (int r = y; r < y + height; r++) {
				for (int c = x; c < x + width; c++) {
					grid[r][c] = false;
				}
			}
			cell_count -= width * height;

			polygon.primitive_shapes.push_back(boost::shared_ptr<util::PrimitiveShape>(new util::PrimitiveRectangle(polygon.mat, cv::Point2f(x_coords[x], y_coords[y]), cv::Point2f(x_coords[x + width], y_coords[y + height]))));
		}
	}

	void RightAngleSimplification::findMaximumRectangle(const std::vector<std::vector<bool>>& grid, const std::vector<float>& x_coords, const std::vector<float>& y_coords, int& x, int& y, int& width, int& height) {
		float max_area = 0;
		
		for (int r = 0; r < grid.size(); r++) {
			for (int c = 0; c < grid[r].size(); c++) {

				for (int r2 = r; r2 < grid.size(); r2++) {
					if (!grid[r2][c]) break;

					for (int c2 = c; c2 < grid[r2].size(); c2++) {
						// check the validity
						bool valid = true;
						for (int r3 = r; r3 <= r2; r3++) {
							if (!grid[r3][c2]) {
								valid = false;
								break;
							}
						}

						if (!valid) break;

						float area = (x_coords[c2 + 1] - x_coords[c]) * (y_coords[r2 + 1] - y_coords[r]);
						if (area > max_area) {
							max_area = area;
							x = c;
							y = r;
							width = c2 - c + 1;
							height = r2 - r + 1;
						}
					}
				}
			}
		}
	}

}