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

		// ToDo:
		// Should we check if the holes are inside the contour?

		return ans;
	}

}