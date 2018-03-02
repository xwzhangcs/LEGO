#include "ContourUtils.h"
#include <iostream>
#include <boost/polygon/polygon.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <list>

namespace util {
	
	/*PrimitiveRectangle::PrimitiveRectangle() {
		mat = cv::Mat_<float>::eye(3, 3);
	}*/

	PrimitiveRectangle::PrimitiveRectangle(const cv::Mat_<float>& mat, const cv::Point2f& min_pt, const cv::Point2f& max_pt) {
		this->mat = mat;
		this->min_pt = min_pt;
		this->max_pt = max_pt;
	}

	boost::shared_ptr<PrimitiveShape> PrimitiveRectangle::clone() const {
		boost::shared_ptr<PrimitiveShape> ans = boost::shared_ptr<PrimitiveShape>(new PrimitiveRectangle(mat, min_pt, max_pt));
		return ans;
	}

	std::vector<cv::Point2f> PrimitiveRectangle::getActualPoints() const {
		std::vector<cv::Point2f> ans(4);

		cv::Mat_<float> p0 = (cv::Mat_<float>(3, 1) << min_pt.x, min_pt.y, 1);
		cv::Mat_<float> q0 = mat * p0;
		cv::Mat_<float> p1 = (cv::Mat_<float>(3, 1) << max_pt.x, min_pt.y, 1);
		cv::Mat_<float> q1 = mat * p1;
		cv::Mat_<float> p2 = (cv::Mat_<float>(3, 1) << max_pt.x, max_pt.y, 1);
		cv::Mat_<float> q2 = mat * p2;
		cv::Mat_<float> p3 = (cv::Mat_<float>(3, 1) << min_pt.x, max_pt.y, 1);
		cv::Mat_<float> q3 = mat * p3;

		ans[0] = cv::Point2f(q0(0, 0), q0(1, 0));
		ans[1] = cv::Point2f(q1(0, 0), q1(1, 0));
		ans[2] = cv::Point2f(q2(0, 0), q2(1, 0));
		ans[3] = cv::Point2f(q3(0, 0), q3(1, 0));

		return ans;
	}

	PrimitiveTriangle::PrimitiveTriangle(const cv::Mat_<float>& mat) {
		this->mat = mat;
	}

	PrimitiveTriangle::PrimitiveTriangle(const cv::Mat_<float>& mat, const std::vector<cv::Point2f>& points) {
		this->mat = mat;
		this->points = points;
	}

	boost::shared_ptr<PrimitiveShape> PrimitiveTriangle::clone() const {
		boost::shared_ptr<PrimitiveShape> ans = boost::shared_ptr<PrimitiveShape>(new PrimitiveTriangle(mat, points));
		return ans;
	}

	std::vector<cv::Point2f> PrimitiveTriangle::getActualPoints() const {
		std::vector<cv::Point2f> ans(points.size());

		for (int i = 0; i < points.size(); i++) {
			cv::Mat_<float> p = (cv::Mat_<float>(3, 1) << points[i].x, points[i].y, 1);
			cv::Mat_<float> q = mat * p;
			ans[i] = cv::Point2f(q(0, 0), q(1, 0));
		}
		return ans;
	}

	PrimitiveCurve::PrimitiveCurve(const cv::Mat_<float>& mat, float theta_start, float theta_end, const cv::Point2f &center, float radius) {
		this->mat = mat;
		this->theta_start = theta_start;
		this->theta_end = theta_end;
		this->center = center;
		this->radius = radius;
	}

	boost::shared_ptr<PrimitiveShape> PrimitiveCurve::clone() const {
		boost::shared_ptr<PrimitiveShape> ans = boost::shared_ptr<PrimitiveShape>(new PrimitiveCurve(mat, theta_start, theta_end, center, radius));
		return ans;
	}

	std::vector<cv::Point2f> PrimitiveCurve::getActualPoints() const {
		float angle_start_end = this->theta_end - this->theta_start;
		float angle_between = 5;
		int num_points = abs(angle_start_end / 5);
		angle_between = angle_start_end / num_points;
		num_points = num_points + 1;
		std::vector<cv::Point2f> ans(num_points);
		for (int k = 0; k < num_points; k++){
			double x = abs(this->radius) * cos(CV_PI * (this->theta_start + angle_between * k) / 180) + this->center.x;
			double y = abs(this->radius) * sin(CV_PI * (this->theta_start + angle_between * k) / 180) + this->center.y;
			cv::Mat_<float> p = (cv::Mat_<float>(3, 1) << x, y, 1);
			cv::Mat_<float> q = mat * p;
			ans[k] = cv::Point2f(q(0, 0), q(1, 0));
		}
		return ans;
	}

	Ring::Ring() {
		mat = cv::Mat_<float>::eye(3, 3);
	}

	Ring::Ring(const Ring& ring) {
		this->mat = ring.mat.clone();
		this->points = ring.points;
	}

	Ring::Ring(const std::vector<cv::Point2f>& points) {
		mat = cv::Mat_<float>::eye(3, 3);
		this->points = points;
	}

	Ring& Ring::operator=(const Ring& ring) {
		this->mat = ring.mat.clone();
		this->points = ring.points;
		return *this;
	}

	Ring& Ring::operator=(const std::vector<cv::Point2f>& points) {
		this->points = points;
		return *this;
	}

	const cv::Point2f& Ring::front() const {
		return points.front();
	}

	cv::Point2f& Ring::front() {
		return points.front();
	}

	const cv::Point2f& Ring::back() const {
		return points.back();
	}

	cv::Point2f& Ring::back() {
		return points.back();
	}

	std::vector<cv::Point2f>::iterator Ring::begin() {
		return points.begin();
	}

	std::vector<cv::Point2f>::iterator Ring::end() {
		return points.end();
	}

	size_t Ring::size() const {
		return points.size();
	}

	void Ring::clear() {
		points.clear();
	}

	void Ring::resize(size_t s) {
		points.resize(s);
	}

	const cv::Point2f& Ring::operator[](int index) const {
		return points[index];
	}

	cv::Point2f& Ring::operator[](int index) {
		return points[index];
	}

	void Ring::push_back(const cv::Point2f& pt) {
		points.push_back(pt);
	}

	void Ring::pop_back() {
		points.pop_back();
	}

	void Ring::erase(std::vector<cv::Point2f>::iterator position) {
		points.erase(position);
	}

	void Ring::translate(float x, float y) {
		for (int i = 0; i < points.size(); i++) {
			points[i].x += x;
			points[i].y += y;
		}
	}

	void Ring::transform(const cv::Mat_<float>& m) {
		mat = m * mat;
		/*
		for (int i = 0; i < points.size(); i++) {
			cv::Mat_<float> p = m * (cv::Mat_<float>(3, 1) << points[i].x, points[i].y, 1);
			points[i].x = p(0, 0);
			points[i].y = p(1, 0);
		}
		*/
	}

	void Ring::clockwise() {
		if (!isClockwise(points)) {
			std::reverse(points.begin(), points.end());
		}
	}

	void Ring::counterClockwise() {
		if (isClockwise(points)) {
			std::reverse(points.begin(), points.end());
		}
	}

	cv::Point2f Ring::getActualPoint(int index) const {
		cv::Mat_<float> pt = (cv::Mat_<float>(3, 1) << points[index].x, points[index].y, 1);
		cv::Mat_<float> pt2 = mat * pt;

		cv::Point2f ans;
		ans.x = pt2(0, 0);
		ans.y = pt2(1, 0);
		return ans;
	}

	Ring Ring::getActualPoints() const {
		Ring ans;
		ans.points.resize(points.size());
		for (int i = 0; i < points.size(); i++) {
			cv::Mat_<float> pt = (cv::Mat_<float>(3, 1) << points[i].x, points[i].y, 1);
			cv::Mat_<float> pt2 = mat * pt;
			ans.points[i].x = pt2(0, 0);
			ans.points[i].y = pt2(1, 0);
		}
		return ans;
	}
	
	bool Ring::isSimple() const {
		CDT cdt;
		CGAL::Partition_traits_2<Kernel>::Polygon_2 polygon;
		for (int i = 0; i < points.size(); i++) {
			polygon.push_back(CDT::Point(points[i].x, points[i].y));
		}
		return polygon.is_simple();
	}

	Polygon Polygon::clone() const {
		Polygon ans;
		ans.mat = mat;
		ans.contour = contour;
		ans.holes = holes;

		ans.primitive_shapes.resize(primitive_shapes.size());
		for (int i = 0; i < primitive_shapes.size(); i++) {
			ans.primitive_shapes[i] = primitive_shapes[i]->clone();
		}

		return ans;
	}

	void Polygon::translate(float x, float y) {
		contour.translate(x, y);
		for (int i = 0; i < holes.size(); i++) {
			holes[i].translate(x, y);
		}
	}

	void Polygon::transform(const cv::Mat_<float>& m) {
		contour.transform(m);
		for (int i = 0; i < holes.size(); i++) {
			holes[i].transform(m);
		}
	}

	void Polygon::clockwise() {
		contour.clockwise();
		for (int i = 0; i < holes.size(); i++) {
			holes[i].clockwise();
		}
	}

	void Polygon::counterClockwise() {
		contour.counterClockwise();
		for (int i = 0; i < holes.size(); i++) {
			holes[i].counterClockwise();
		}
	}

	bool isClockwise(const std::vector<cv::Point2f>& polygon) {
		float S = 0;
		for (int i = 0; i < polygon.size(); i++) {
			int next = (i + 1) % polygon.size();
			S += (polygon[next].x - polygon[i].x) * (polygon[next].y + polygon[i].y);
		}
		return S > 0;
	}

	void clockwise(std::vector<cv::Point2f>& polygon) {
		if (!isClockwise(polygon)) {
			std::reverse(polygon.begin(), polygon.end());
		}
	}

	void counterClockwise(std::vector<cv::Point2f>& polygon) {
		if (isClockwise(polygon)) {
			std::reverse(polygon.begin(), polygon.end());
		}
	}

	bool isSimple(const Ring& points){
		CDT cdt;
		CGAL::Partition_traits_2<Kernel>::Polygon_2 polygon;
		for (int i = 0; i < points.size(); i++) {
			polygon.push_back(CDT::Point(points[i].x, points[i].y));
		}
		return polygon.is_simple();
	}

	/**
	 * Remove the redundant points that have the same coordinates, and remove the point that are collinear to the adjacent points.
	 */
	std::vector<cv::Point> removeRedundantPoint(const std::vector<cv::Point>& polygon) {
		std::vector<cv::Point> ans;
		if (polygon.size() == 0) return ans;

		ans.push_back(polygon[0]);
		for (int i = 1; i < polygon.size(); i++) {
			if (polygon[i] != polygon[i - 1]) {
				ans.push_back(polygon[i]);
			}
		}
		if (ans.size() > 1 && ans.back() == ans.front()) ans.pop_back();

		for (int i = 0; i < ans.size() && ans.size() >= 3; ) {
			int prev = (i - 1 + ans.size()) % ans.size();
			int next = (i + 1) % ans.size();
			if (dotProduct(ans[i] - ans[prev], ans[next] - ans[i]) > 0 && std::abs(crossProduct(ans[i] - ans[prev], ans[next] - ans[i])) < 0.0001) {
				ans.erase(ans.begin() + i);
			}
			else {
				i++;
			}
		}

		return ans;
	}

	Ring removeRedundantPoint(const Ring& polygon) {
		Ring ans;
		if (polygon.size() == 0) return ans;

		ans.push_back(polygon[0]);
		for (int i = 1; i < polygon.size(); i++) {
			if (polygon[i] != polygon[i - 1]) {
				ans.push_back(polygon[i]);
			}
		}
		if (ans.size() > 1 && ans.back() == ans.front()) ans.pop_back();

		for (int i = 0; i < ans.size() && ans.size() >= 3;) {
			int prev = (i - 1 + ans.size()) % ans.size();
			int next = (i + 1) % ans.size();
			if (dotProduct(ans[i] - ans[prev], ans[next] - ans[i]) > 0 && std::abs(crossProduct(ans[i] - ans[prev], ans[next] - ans[i])) < 0.0001) {
				ans.erase(ans.begin() + i);
			}
			else {
				i++;
			}
		}

		return ans;
	}

	cv::Rect boundingBox(const std::vector<cv::Point>& polygon) {
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

	cv::Rect boundingBox(const std::vector<cv::Point2f>& polygon) {
		int min_x = std::numeric_limits<int>::max();
		int max_x = -std::numeric_limits<int>::max();
		int min_y = std::numeric_limits<int>::max();
		int max_y = -std::numeric_limits<int>::max();
		for (int i = 0; i < polygon.size(); i++) {
			min_x = std::min(min_x, (int)polygon[i].x);
			max_x = std::max(max_x, (int)polygon[i].x);
			min_y = std::min(min_y, (int)polygon[i].y);
			max_y = std::max(max_y, (int)polygon[i].y);
		}

		return cv::Rect(min_x, min_y, max_x - min_x + 1, max_y - min_y + 1);
	}

	bool withinPolygon(const cv::Point2f& pt, const Polygon& polygon) {
		if (!withinPolygon(pt, polygon.contour)) return false;

		for (int i = 0; i < polygon.holes.size(); i++) {
			if (withinPolygon(pt, polygon.holes[i])) return false;
		}

		return true;
	}

	bool withinPolygon(const cv::Point2f& pt, const Ring& ring) {
		boost::geometry::model::ring<boost::geometry::model::d2::point_xy<float>> contour;
		for (int i = 0; i < ring.size(); ++i) {
			contour.push_back(boost::geometry::model::d2::point_xy<float>(ring[i].x, ring[i].y));
		}
		boost::geometry::correct(contour);

		return boost::geometry::within(boost::geometry::model::d2::point_xy<float>(pt.x, pt.y), contour);
	}

	/**
	* Calculate the intersection over union (IOU) inside the specified roi.
	* The images have to be CV_8U type.
	*/
	double calculateIOU(const cv::Mat_<uchar>& img, const cv::Mat_<uchar>& img2) {
		double union_cnt = 0;
		double inter_cnt = 0;
		for (int r = 0; r < img.rows; r++) {
			for (int c = 0; c < img.cols; c++) {
				union_cnt += std::max(img.at<uchar>(r, c), img2.at<uchar>(r, c));
				inter_cnt += std::min(img.at<uchar>(r, c), img2.at<uchar>(r, c));
			}
		}

		return (double)inter_cnt / union_cnt;
	}

	/**
	* Calculate the intersection over union (IOU) inside the specified roi.
	* The images have to be CV_8U type.
	* Check the IOU only within the specified region, rect.
	*/
	double calculateIOU(const cv::Mat_<uchar>& img1, const cv::Mat_<uchar>& img2, const cv::Rect& rect) {
		double union_cnt = 0;
		double inter_cnt = 0;

		for (int r = rect.y; r < rect.y + rect.height; r++) {
			for (int c = rect.x; c < rect.x + rect.width; c++) {
				union_cnt += std::max(img1.at<uchar>(r, c), img2.at<uchar>(r, c));
				inter_cnt += std::min(img1.at<uchar>(r, c), img2.at<uchar>(r, c));
			}
		}

		return (float)inter_cnt / union_cnt;
	}

	/**
	 * Calculate the intersection over union (IOU) between two polygons.
	 * Since calculating the polygon intersecion and union exactly is very expensive,
	 * we resort to an image-based approach that can quickly calculate the approximate IOU.
	 */
	double calculateIOU(const Polygon& polygon1, const Polygon& polygon2) {
		Ring contour1 = polygon1.contour.getActualPoints();
		Ring contour2 = polygon2.contour.getActualPoints();

		int min_x = INT_MAX;
		int min_y = INT_MAX;
		int max_x = INT_MIN;
		int max_y = INT_MIN;
		for (int i = 0; i < contour1.size(); i++) {
			min_x = std::min(min_x, (int)contour1[i].x);
			min_y = std::min(min_y, (int)contour1[i].y);
			max_x = std::max(max_x, (int)(contour1[i].x + 0.5));
			max_y = std::max(max_y, (int)(contour1[i].y + 0.5));
		}
		for (int i = 0; i < contour2.size(); i++) {
			min_x = std::min(min_x, (int)contour2[i].x);
			min_y = std::min(min_y, (int)contour2[i].y);
			max_x = std::max(max_x, (int)(contour2[i].x + 0.5));
			max_y = std::max(max_y, (int)(contour2[i].y + 0.5));
		}

		cv::Mat_<uchar> img1 = cv::Mat_<uchar>::zeros(max_y - min_y + 1, max_x - min_x + 1);
		
		std::vector<std::vector<cv::Point>> contour_points1(1 + polygon1.holes.size());
		contour_points1[0].resize(contour1.size());
		for (int i = 0; i < contour1.size(); i++) {
			contour_points1[0][i] = cv::Point(contour1[i].x - min_x, contour1[i].y - min_y);
		}
		for (int i = 0; i < polygon1.holes.size(); i++) {
			Ring hole = polygon1.holes[i].getActualPoints();
			contour_points1[i + 1].resize(hole.size());
			for (int j = 0; j < hole.size(); j++) {
				contour_points1[i + 1][j] = cv::Point(hole[j].x - min_x, hole[j].y - min_y);
			}
		}
		cv::fillPoly(img1, contour_points1, cv::Scalar(255), cv::LINE_4);

		cv::Mat_<uchar> img2 = cv::Mat_<uchar>::zeros(max_y - min_y + 1, max_x - min_x + 1);

		std::vector<std::vector<cv::Point>> contour_points2(1 + polygon2.holes.size());
		contour_points2[0].resize(contour2.size());
		for (int i = 0; i < contour2.size(); i++) {
			contour_points2[0][i] = cv::Point(contour2[i].x - min_x, contour2[i].y - min_y);
		}
		for (int i = 0; i < polygon2.holes.size(); i++) {
			Ring hole = polygon2.holes[i].getActualPoints();
			contour_points2[i + 1].resize(hole.size());
			for (int j = 0; j < hole.size(); j++) {
				contour_points2[i + 1][j] = cv::Point(hole[j].x - min_x, hole[j].y - min_y);
			}
		}
		cv::fillPoly(img2, contour_points2, cv::Scalar(255), cv::LINE_4);

		int inter_cnt = 0;
		int union_cnt = 0;
		for (int r = 0; r < img1.rows; r++) {
			for (int c = 0; c < img1.cols; c++) {
				if (img1(r, c) == 255 && img2(r, c) == 255) inter_cnt++;
				if (img1(r, c) == 255 || img2(r, c) == 255) union_cnt++;
			}
		}

		return (double)inter_cnt / union_cnt;
	}

	/*
	double calculateIOU(const Polygon& polygon1, const Polygon& polygon2) {
		CGAL::Polygon_2<Kernel> P1;
		for (int i = 0; i < polygon1.contour.size(); i++) {
			P1.push_back(Kernel::Point_2(polygon1.contour[i].x, polygon1.contour[i].y));
		}
		if (P1.is_clockwise_oriented()) P1.reverse_orientation();
		std::vector<CGAL::Polygon_2<Kernel>> P1_holes(polygon1.holes.size());
		for (int i = 0; i < polygon1.holes.size(); i++) {
			for (int j = 0; j < polygon1.holes[i].size(); j++) {
				P1_holes[i].push_back(Kernel::Point_2(polygon1.holes[i][j].x, polygon1.holes[i][j].y));
			}
			if (P1_holes[i].is_counterclockwise_oriented()) P1_holes[i].reverse_orientation();
		}
		CGAL::Polygon_with_holes_2<Kernel> PH1(P1, P1_holes.begin(), P1_holes.end());

		CGAL::Polygon_2<Kernel> P2;
		for (int i = 0; i < polygon2.contour.size(); i++) {
			P2.push_back(Kernel::Point_2(polygon2.contour[i].x, polygon2.contour[i].y));
		}
		if (P2.is_clockwise_oriented()) P2.reverse_orientation();
		std::vector<CGAL::Polygon_2<Kernel>> P2_holes(polygon2.holes.size());
		for (int i = 0; i < polygon2.holes.size(); i++) {
			for (int j = 0; j < polygon2.holes[i].size(); j++) {
				P2_holes[i].push_back(Kernel::Point_2(polygon2.holes[i][j].x, polygon2.holes[i][j].y));
			}
			if (P2_holes[i].is_counterclockwise_oriented()) P2_holes[i].reverse_orientation();
		}
		CGAL::Polygon_with_holes_2<Kernel> PH2(P2, P2_holes.begin(), P2_holes.end());

		// calculate union
		CGAL::Polygon_with_holes_2<Kernel> unionR;
		if (!CGAL::join(PH1, PH2, unionR)) {
			// there is no intersection.
			return 0;
		}

		double union_area = std::abs(unionR.outer_boundary().area().exact().to_double());
		for (auto it = unionR.holes_begin(); it != unionR.holes_end(); it++) {
			union_area -= std::abs(it->area().exact().to_double());
		}

		// calculate intersection
		std::list<CGAL::Polygon_with_holes_2<Kernel>> intR;
		std::list<CGAL::Polygon_with_holes_2<Kernel>>::const_iterator it;

		CGAL::intersection(PH1, PH2, std::back_inserter(intR));

		double intersection_area = 0;
		for (it = intR.begin(); it != intR.end(); ++it) {
			intersection_area += std::abs(it->outer_boundary().area().exact().to_double());
			for (auto it2 = it->holes_begin(); it2 != it->holes_end(); it2++) {
				intersection_area -= std::abs(it2->area().exact().to_double());
			}
			
		}

		return intersection_area / union_area;
	}
	*/

	double calculateIOU(const std::vector<cv::Point2f>& polygon1, const std::vector<cv::Point2f>& polygon2) {
		int min_x = INT_MAX;
		int min_y = INT_MAX;
		int max_x = INT_MIN;
		int max_y = INT_MIN;
		for (int i = 0; i < polygon1.size(); i++) {
			min_x = std::min(min_x, (int)polygon1[i].x);
			min_y = std::min(min_y, (int)polygon1[i].y);
			max_x = std::max(max_x, (int)(polygon1[i].x + 0.5));
			max_y = std::max(max_y, (int)(polygon1[i].y + 0.5));
		}
		for (int i = 0; i < polygon2.size(); i++) {
			min_x = std::min(min_x, (int)polygon2[i].x);
			min_y = std::min(min_y, (int)polygon2[i].y);
			max_x = std::max(max_x, (int)(polygon2[i].x + 0.5));
			max_y = std::max(max_y, (int)(polygon2[i].y + 0.5));
		}

		cv::Mat_<uchar> img1 = cv::Mat_<uchar>::zeros(max_y - min_y + 1, max_x - min_x + 1);

		std::vector<std::vector<cv::Point>> contour_points1(1);
		contour_points1[0].resize(polygon1.size());
		for (int i = 0; i < polygon1.size(); i++) {
			contour_points1[0][i] = cv::Point(polygon1[i].x - min_x, polygon1[i].y - min_y);
		}
		cv::fillPoly(img1, contour_points1, cv::Scalar(255), cv::LINE_4);

		cv::Mat_<uchar> img2 = cv::Mat_<uchar>::zeros(max_y - min_y + 1, max_x - min_x + 1);

		std::vector<std::vector<cv::Point>> contour_points2(1);
		contour_points2[0].resize(polygon2.size());
		for (int i = 0; i < polygon2.size(); i++) {
			contour_points2[0][i] = cv::Point(polygon2[i].x - min_x, polygon2[i].y - min_y);
		}
		cv::fillPoly(img2, contour_points2, cv::Scalar(255), cv::LINE_4);

		int inter_cnt = 0;
		int union_cnt = 0;
		for (int r = 0; r < img1.rows; r++) {
			for (int c = 0; c < img1.cols; c++) {
				if (img1(r, c) == 255 && img2(r, c) == 255) inter_cnt++;
				if (img1(r, c) == 255 || img2(r, c) == 255) union_cnt++;
			}
		}

		return (double)inter_cnt / union_cnt;
	}

	double calculateArea(const Polygon& polygon) {
		double ans = cv::contourArea(polygon.contour.points);
		for (int i = 0; i < polygon.holes.size(); i++) {
			ans -= cv::contourArea(polygon.holes[i].points);
		}
		return ans;
	}

	double calculateArea(const cv::Mat_<uchar>& img) {
		return cv::sum(img)[0] / 255;
	}

	/**
	 * Helper function to extract contours from the input image.
	 * The image has to be of type CV_8U, and has values either 0 or 255.
	 * Note that the input image is not modified by this function.
	 */
	std::vector<Polygon> findContours(const cv::Mat_<uchar>& img, bool add_right_corner) {
		std::vector<Polygon> ans;

		// resize x2
		cv::Mat_<uchar> img2;
		cv::resize(img, img2, cv::Size(img.cols * 2, img.rows * 2), 0, 0, cv::INTER_NEAREST);

		// add padding
		cv::Mat_<uchar> padded = cv::Mat_<uchar>::zeros(img2.rows + 1, img2.cols + 1);
		img2.copyTo(padded(cv::Rect(0, 0, img2.cols, img2.rows)));

		// dilate image
		cv::Mat_<uchar> kernel = (cv::Mat_<uchar>(3, 3) << 1, 1, 0, 1, 1, 0, 0, 0, 0);
		cv::dilate(padded, padded, kernel);
		
		// extract contours
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(padded, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

		for (int i = 0; i < hierarchy.size(); i++) {
			if (hierarchy[i][3] != -1) continue;
			if (contours[i].size() < 3) continue;

			Polygon polygon;
			if (add_right_corner) {
				Ring contour = addCornerToOpenCVContour(contours[i], padded);
				polygon.contour.resize(contour.size());
				for (int j = 0; j < contour.size(); j++) {
					polygon.contour[j] = cv::Point2f(contour[j].x * 0.5, contour[j].y * 0.5);
				}
			}
			else {
				polygon.contour.resize(contours[i].size());
				for (int j = 0; j < contours[i].size(); j++) {
					polygon.contour[j] = cv::Point2f(contours[i][j].x * 0.5, contours[i][j].y * 0.5);
				}
			}
			
			if (polygon.contour.size() >= 3) {
				// obtain all the holes inside this contour
				int hole_id = hierarchy[i][2];
				while (hole_id != -1) {
					if (add_right_corner) {
						Ring hole = addCornerToOpenCVContour(contours[hole_id], img2);
						for (int j = 0; j < hole.size(); j++) {
							hole[j] = cv::Point2f(hole[j].x * 0.5, hole[j].y * 0.5);
						}
						polygon.holes.push_back(hole);
					}
					else {
						Ring hole;
						hole.resize(contours[hole_id].size());
						for (int j = 0; j < contours[hole_id].size(); j++) {
							hole[j] = cv::Point2f(contours[hole_id][j].x * 0.5, contours[hole_id][j].y * 0.5);
						}
						polygon.holes.push_back(hole);
					}
					hole_id = hierarchy[hole_id][0];
				}

				ans.push_back(polygon);
			}
		}

		return ans;
	}

	Ring addCornerToOpenCVContour(const std::vector<cv::Point>& polygon, const cv::Mat_<uchar>& img) {
		Ring ans;
		
		for (int i = 0; i < polygon.size(); i++) {
			ans.push_back(polygon[i]);

			int next = (i + 1) % polygon.size();

			// check if the edge is diagonal
			if (std::abs(polygon[next].x - polygon[i].x) == 1 && std::abs(polygon[next].y - polygon[i].y) == 1) {
				// add a right angle corner
				cv::Point2f p(polygon[next].x, polygon[i].y);
				if (img.at<uchar>(p.y, p.x) == 255) ans.push_back(p);
				else {
					cv::Point2f p(polygon[i].x, polygon[next].y);
					if (img.at<uchar>(p.y, p.x) == 255) ans.push_back(p);
					else {
						cv::Point2f p(std::max(polygon[i].x, polygon[next].x), std::min(polygon[i].y, polygon[next].y));
						if ((p.x != polygon[i].x || p.y != polygon[i].y) && (p.x != polygon[next].x || p.y != polygon[next].y)) {
							ans.push_back(p);
						}
						else {
							cv::Point2f p(std::min(polygon[i].x, polygon[next].x), std::min(polygon[i].y, polygon[next].y));
							ans.push_back(p);
						}
					}
				}
			}
		}

		return removeRedundantPoint(ans);
	}

	/**
 	 * My custom contour extraction
	 * Currently, only the right-angle corner is supported, and only one contour is supported.
	 *
	 *
	 * @param img		input single-channel image (0 - background, 255 - footprint)
	 * @param contour	output contour polygon
	 */
	void findContour(const cv::Mat_<uchar>& img, std::vector<cv::Point>& contour) {
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
	 * Create image from the contour.
	 */
	void createImageFromContour(int width, int height, const std::vector<cv::Point>& contour, const cv::Point& offset, cv::Mat_<uchar>& result) {
		result = cv::Mat_<uchar>::zeros(height * 2, width * 2);
		std::vector<std::vector<cv::Point>> contour_points(1);

		contour_points[0].resize(contour.size());
		for (int i = 0; i < contour.size(); i++) {
			contour_points[0][i] = cv::Point((contour[i].x + offset.x) * 2, (contour[i].y + offset.y) * 2);
		}

		cv::fillPoly(result, contour_points, cv::Scalar(255), cv::LINE_4);

		// erode image
		cv::Mat_<uchar> kernel = (cv::Mat_<uchar>(3, 3) << 0, 0, 0, 0, 0, 1, 0, 1, 1);
		cv::erode(result, result, kernel);

		cv::resize(result, result, cv::Size(width, height), 0, 0, cv::INTER_NEAREST);
	}

	void createImageFromPolygon(int width, int height, const Polygon& polygon, const cv::Point& offset, cv::Mat_<uchar>& result) {
		result = cv::Mat_<uchar>::zeros(height * 2, width * 2);
		std::vector<std::vector<cv::Point>> contour_points(1 + polygon.holes.size());

		contour_points[0].resize(polygon.contour.size());
		for (int i = 0; i < polygon.contour.size(); i++) {
			contour_points[0][i] = cv::Point((polygon.contour[i].x + offset.x) * 2, (polygon.contour[i].y + offset.y) * 2);
		}
		for (int i = 0; i < polygon.holes.size(); i++) {
			contour_points[i + 1].resize(polygon.holes[i].size());
			for (int j = 0; j < polygon.holes[i].size(); j++) {
				contour_points[i + 1][j] = cv::Point((polygon.holes[i][j].x + offset.x) * 2, (polygon.holes[i][j].y + offset.y) * 2);
			}
		}
		cv::fillPoly(result, contour_points, cv::Scalar(255), cv::LINE_4);

		// erode image
		cv::Mat_<uchar> kernel = (cv::Mat_<uchar>(3, 3) << 0, 0, 0, 0, 0, 1, 0, 1, 1);
		cv::erode(result, result, kernel);

		cv::resize(result, result, cv::Size(width, height), 0, 0, cv::INTER_NEAREST);
	}

	void snapPolygon(const std::vector<cv::Point2f>& ref_polygon, std::vector<cv::Point2f>& polygon, float snap_vertex_threshold, float snap_edge_threshold) {
		for (int i = 0; i < polygon.size(); i++) {
			// find the closest point
			double min_length = std::numeric_limits<double>::max();
			int min_index = -1;
			for (int j = 0; j < ref_polygon.size(); j++) {
				double length = cv::norm(polygon[i] - ref_polygon[j]);
				if (length < min_length) {
					min_length = length;
					min_index = j;
				}
			}

			if (min_length < snap_vertex_threshold) {
				polygon[i] = ref_polygon[min_index];
			}
			else {
				min_length = std::numeric_limits<double>::max();
				cv::Point2f min_pt;
				// find the closest edge
				for (int j = 0; j < ref_polygon.size(); j++) {
					int next = (j + 1) % ref_polygon.size();
					cv::Point2f pt;
					double length = closestPoint(ref_polygon[j], ref_polygon[next], polygon[i], true, pt);
					if (length < min_length) {
						min_length = length;
						min_pt = pt;
					}
				}

				if (min_length < snap_edge_threshold) {
					polygon[i] = min_pt;
				}
			}
		}
	}

	float crossProduct(const cv::Point2f& v1, const cv::Point2f& v2) {
		return v1.x * v2.y - v1.y * v2.x;
	}

	float dotProduct(const cv::Point2f& v1, const cv::Point2f& v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}

	/*
	 * Return the distance from segment ab to point c.
	 */
	float closestPoint(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c, bool segmentOnly, cv::Point2f& pt) {
		float r_numerator = dotProduct(c - a, b - a);
		float r_denomenator = dotProduct(b - a, b - a);

		if (r_denomenator <= 0.0f) {
			return cv::norm(a - c);
		}

		float r = r_numerator / r_denomenator;

		if (segmentOnly && (r < 0 || r > 1)) {
			float dist1 = std::hypot(c.x - a.x, c.y - a.y);
			float dist2 = std::hypot(c.x - b.x, c.y - b.y);
			if (dist1 < dist2) {
				pt = a;
				return dist1;
			}
			else {
				pt = b;
				return dist2;
			}
		}
		else {
			pt = a + (b - a) * r;
			return std::abs(crossProduct(c - a, b - a)) / sqrt(r_denomenator);
		}
	}

	/**
	 * Triangulate the polygon into triangles using the Delaunay triangulation.
	 * This function requires the polygon without any self-intersection. If the polygon has
	 * any self-intersection, this function will return no triangle.
	 *
	 * @param points		contour
	 * @return			list of triangles
	 */
	std::vector<std::vector<cv::Point2f>> tessellate(const Ring& points) {
		return tessellate(points, {});
	}

	/**
	 * Triangulate the polygon with holes into triangles using the Delaunay triangulation.
	 * This function requires the polygon without any self-intersection. If the polygon has
	 * any self-intersection, this function will return no triangle.
	 *
	 * @param points	contour
	 * @param holes		holes
	 * @return			list of triangles
	 */
	std::vector<std::vector<cv::Point2f>> tessellate(const Ring& points, const std::vector<Ring>& holes) {
		std::vector<std::vector<cv::Point2f>> ans;

		// Don't triangulate if there are only less than 3 points.
		if (points.size() == 0) {
			return{};
		}
		else if (points.size() <= 2) {
			return{ points.points };
		}

		// Insert the polygons into a constrained triangulation.
		CDT cdt;
		CGAL::Partition_traits_2<Kernel>::Polygon_2 polygon;
		for (int i = 0; i < points.size(); i++) {
			polygon.push_back(CDT::Point(points[i].x, points[i].y));
		}

		if (polygon.is_simple()) {
			cdt.insert_constraint(polygon.vertices_begin(), polygon.vertices_end(), true);
			for (int i = 0; i < holes.size(); i++) {
				CGAL::Partition_traits_2<Kernel>::Polygon_2 polygon;
				for (int j = 0; j < holes[i].size(); j++) {
					polygon.push_back(CDT::Point(holes[i][j].x, holes[i][j].y));
				}
				cdt.insert_constraint(polygon.vertices_begin(), polygon.vertices_end(), true);
			}

			//Mark facets that are inside the domain bounded by the polygon
			mark_domains(cdt);

			for (CDT::Finite_faces_iterator fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
				if (fit->info().in_domain()) {
					std::vector<cv::Point2f> pol;
					for (int i = 0; i < 3; i++) {
						CDT::Vertex_handle vh = fit->vertex(i);
						pol.push_back(cv::Point2f(vh->point().exact().x().to_double(), vh->point().exact().y().to_double()));
					}

					util::counterClockwise(pol);
					ans.push_back(pol);
				}
			}
		}

		return ans;
	}

	void mark_domains(CDT& ct, CDT::Face_handle start, int index, std::list<CDT::Edge>& border) {
		if (start->info().nesting_level != -1){
			return;
		}
		std::list<CDT::Face_handle> queue;
		queue.push_back(start);
		while (!queue.empty()){
			CDT::Face_handle fh = queue.front();
			queue.pop_front();
			if (fh->info().nesting_level == -1){
				fh->info().nesting_level = index;
				for (int i = 0; i < 3; i++){
					CDT::Edge e(fh, i);
					CDT::Face_handle n = fh->neighbor(i);
					if (n->info().nesting_level == -1){
						if (ct.is_constrained(e)) {
							border.push_back(e);


						}
						else queue.push_back(n);
					}
				}
			}
		}
	}

	//explore set of facets connected with non constrained edges,
	//and attribute to each such set a nesting level.
	//We start from facets incident to the infinite vertex, with a nesting
	//level of 0. Then we recursively consider the non-explored facets incident 
	//to constrained edges bounding the former set and increase the nesting level by 1.
	//Facets in the domain are those with an odd nesting level.
	void mark_domains(CDT& cdt) {
		for (CDT::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); ++it){
			it->info().nesting_level = -1;
		}
		std::list<CDT::Edge> border;
		mark_domains(cdt, cdt.infinite_face(), 0, border);
		while (!border.empty()){
			CDT::Edge e = border.front();
			border.pop_front();
			CDT::Face_handle n = e.first->neighbor(e.second);
			if (n->info().nesting_level == -1){
				mark_domains(cdt, n, e.first->info().nesting_level + 1, border);
			}
		}
	}

}
