#include "OurCustomSimplification.h"
#include "ContourUtils.h"

namespace simp {

	OurCustomSimplification::OurCustomSimplification(const std::vector<cv::Mat>& voxel_data, int resolution, double layering_threshold) {
		this->voxel_data = voxel_data;
		this->resolution = resolution;
		this->layering_threshold = layering_threshold;
		this->size = cv::Size(voxel_data[0].cols, voxel_data[0].rows);
	}

	void OurCustomSimplification::simplify(std::vector<Building>& buildings) {
		std::vector<util::Polygon> polygons = util::findContours(voxel_data[5]);
		for (int i = 0; i < polygons.size(); i++) {
			calculateBuilding(polygons[i], 5, -1, -1, -1, buildings);
		}

		std::cout << "Processing buildings has been finished." << std::endl;
	}

	void OurCustomSimplification::calculateBuilding(const util::Polygon& polygon, int height, double angle, int dx, int dy, std::vector<Building>& buildings) {
		// calculate the bounding box
		cv::Rect bbox = util::boundingBox(polygon.contour);

		// have 1px as margin
		bbox.x = std::max(0, bbox.x - 1);
		bbox.y = std::max(0, bbox.y - 1);
		bbox.width = std::min(size.width - bbox.x - 1, bbox.width + 2);
		bbox.height = std::min(size.height - bbox.y - 1, bbox.height + 2);

		// find the height at which the contour drastically changes
		int next_height = findDrasticChange(height, polygon, layering_threshold);

		// calculate building by simplifying the contour and holes
		try {
			Building building = calculateBuildingComponent(polygon, height, next_height, angle, dx, dy);
			buildings.push_back(building);

			if (next_height >= voxel_data.size()) return;

			// crop the image of the next height
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
					if (pointPolygonTest(polygon.contour, next_contour[j], false) < 0) cnt_outside++;
				}

				if (cnt_outside < next_contour.size() / 2) {
					// for upper layers, recursive call this function to construct building components
					calculateBuilding(polygon, next_height, angle, dx, dy, buildings);
				}
			}
		}
		catch (const char* ex) {
		}
	}

	/**
	* Calculate the building geometry by simplifying the specified footprint and holes using OpenCV function.
	*/
	Building OurCustomSimplification::calculateBuildingComponent(const util::Polygon& polygon, int bottom_height, int top_height, double& angle, int& dx, int& dy) {
		std::vector<cv::Point2f> simplified_contour;
		if (angle == -1) {
			std::tuple<double, int, int> best_mat = simplify(polygon.contour, simplified_contour, resolution);
			angle = std::get<0>(best_mat);
			dx = std::get<1>(best_mat);
			dy = std::get<2>(best_mat);
		}
		else {
			simplify(polygon.contour, simplified_contour, resolution, angle, dx, dy);
		}

		if (simplified_contour.size() < 3) throw "Invalid contour";

		// create a building object
		Building building;
		building.bottom_height = bottom_height;
		building.top_height = top_height;

		building.footprint.resize(simplified_contour.size());
		for (int i = 0; i < simplified_contour.size(); i++) {
			building.footprint[i] = cv::Point2f(simplified_contour[i].x - size.width * 0.5, size.height * 0.5 - simplified_contour[i].y);
		}

		// simplify the hole as well
		building.holes.resize(polygon.holes.size());
		for (int i = 0; i < polygon.holes.size(); i++) {
			std::vector<cv::Point2f> simplified_hole;
			std::vector<cv::Point2f> simplified_hole2;
			simplify(polygon.holes[i], simplified_hole, resolution);
			if (simplified_hole.size() >= 3) {
				building.holes[i].resize(simplified_hole.size());
				for (int j = 0; j < simplified_hole.size(); j++) {
					building.holes[i][j] = cv::Point2f(simplified_hole[j].x - size.width * 0.5, size.height * 0.5 - simplified_hole[j].y);
				}
			}
		}

		return building;
	}

	int OurCustomSimplification::findDrasticChange(int height, const util::Polygon& polygon, double threshold) {
		// calculate the bounding box
		cv::Rect bbox = util::boundingBox(polygon.contour);

		for (int i = height + 1; i < voxel_data.size(); i++) {
			double iou = util::calculateIOU(voxel_data[height], voxel_data[i], bbox);
			if (iou < threshold) {
				return i;
			}
		}

		return voxel_data.size();
	}

	/**
	* Simplify and regularize a polygon
	*
	* @param contour		input contour polygon
	* @param result		output regularized polygon
	* @param resolution	resolution which defines how much simplified
	* @return				best angle, dx, and dy that yiled the resulting simplified polygon
	*/
	std::tuple<double, int, int> OurCustomSimplification::simplify(const std::vector<cv::Point>& contour, std::vector<cv::Point2f>& result, double resolution) {
		result.clear();

		double min_cost = std::numeric_limits<double>::max();
		double best_angle;
		int best_dx;
		int best_dy;

		for (double angle = 0; angle < 90; angle += 10) {
			//std::cout << "angle=" << angle << std::endl;

			for (int dx = 0; dx < resolution; dx++) {
				for (int dy = 0; dy < resolution; dy++) {
					std::vector<cv::Point2f> simplified_contour;
					double cost = simplify(contour, simplified_contour, resolution, angle, dx, dy);

					if (cost < min_cost) {
						min_cost = cost;
						best_angle = angle;
						best_dx = dx;
						best_dy = dy;

						result = simplified_contour;
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
	double OurCustomSimplification::simplify(const std::vector<cv::Point>& contour, std::vector<cv::Point2f>& result, double resolution, double angle, int dx, int dy) {
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

		// skip the points that are redundant
		std::vector<cv::Point> simplified_small_aa_contour = util::removeRedundantPoint(small_aa_polygon);

		std::vector<cv::Point> simplified_small_aa_contour2;
		for (int i = 0; i < simplified_small_aa_contour.size(); i++) {
			int prev = (i - 1 + simplified_small_aa_contour.size()) % simplified_small_aa_contour.size();
			int next = (i + 1) % simplified_small_aa_contour.size();
			cv::Point v1 = simplified_small_aa_contour[i] - simplified_small_aa_contour[prev];
			cv::Point v2 = simplified_small_aa_contour[next] - simplified_small_aa_contour[i];
			if (std::abs(v1.x * v2.y - v1.y * v2.x) >= 1) {
				simplified_small_aa_contour2.push_back(simplified_small_aa_contour[i]);
			}
		}
		simplified_small_aa_contour2 = util::removeRedundantPoint(simplified_small_aa_contour2);

		if (simplified_small_aa_contour2.size() < 3) {
			result.clear();
			return std::numeric_limits<double>::max();
		}

		// offset back and scale up the simplified scale-down polygon
		std::vector<cv::Point> simplified_aa_contour(simplified_small_aa_contour2.size());
		for (int i = 0; i < simplified_small_aa_contour2.size(); i++) {
			simplified_aa_contour[i] = cv::Point(simplified_small_aa_contour2[i].x * resolution, simplified_small_aa_contour2[i].y * resolution);
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