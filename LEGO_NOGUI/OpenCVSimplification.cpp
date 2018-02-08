#include "OpenCVSimplification.h"
#include "ContourUtils.h"

namespace simp {

	/**
	 * Simplify the footprint of the layer.
	 *
	 * @param slices	slice images of the layer
	 * @param epsilon	simplification parameter
	 * @return			simplified footprint
	 */
	util::Polygon OpenCVSimplification::simplify(const cv::Mat& slice, float epsilon) {
		// make sure there is a building in the layer
		std::vector<util::Polygon> polygons = util::findContours(slice);
		if (polygons.size() == 0) throw "No building is found.";

		util::Polygon ans;
		cv::approxPolyDP(polygons[0].contour.points, ans.contour.points, epsilon, true);
		if (ans.contour.size() < 3) ans.contour = polygons[0].contour;
	
		// simplify the hole as well
		for (int i = 0; i < polygons[0].holes.size(); i++) {
			util::Ring simplified_hole;
			cv::approxPolyDP(polygons[0].holes[i].points, simplified_hole.points, epsilon, true);
			if (simplified_hole.size() >= 3) {
				ans.holes.push_back(simplified_hole);
			}
		}

		// The transfomration matrix should be same for the external contour and the internal holes
		// because for OpenCV simplification, transformation is just to trasnform from OpenCV image coordinates
		// to the world coordinate system.
		ans.mat = ans.contour.mat;

		// ToDo:
		// Should we check if the holes are inside the contour?

		decomposePolygon(ans);

		return ans;
	}

	void OpenCVSimplification::decomposePolygon(util::Polygon& polygon) {
		std::vector<std::vector<cv::Point2f>> polygons;

		if (polygon.holes.size() == 0) {
			polygons = util::tessellate(polygon.contour);
		}
		else {
			polygons = util::tessellate(polygon.contour, polygon.holes);
		}

		polygon.primitive_shapes.clear();

		for (int i = 0; i < polygons.size(); i++) {
			util::clockwise(polygons[i]);

			for (int j = 1; j < polygons[i].size() - 1; j++) {
				boost::shared_ptr<util::PrimitiveTriangle> pol = boost::shared_ptr<util::PrimitiveTriangle>(new util::PrimitiveTriangle(polygon.mat));
				pol->points.push_back(cv::Point2f(polygons[i][0].x, polygons[i][0].y));
				pol->points.push_back(cv::Point2f(polygons[i][j].x, polygons[i][j].y));
				pol->points.push_back(cv::Point2f(polygons[i][j + 1].x, polygons[i][j + 1].y));
				polygon.primitive_shapes.push_back(pol);
			}
		}
	}

}