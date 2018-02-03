#include "OpenCVSimplification.h"
#include "ContourUtils.h"

namespace simp {

	/**
	* Calculate the building geometry by simplifying the specified footprint and holes using OpenCV function.
	*/
	util::Polygon OpenCVSimplification::simplify(const std::vector<cv::Mat>& slices, float epsilon) {
		// select the best slice that has the best IOU with all the slices in the layer
		double best_iou = 0;
		int best_slice = -1;
		for (int i = 0; i < slices.size(); i++) {
			double iou = 0;
			for (int j = 0; j < slices.size(); j++) {
				// calculate IOU
				iou += util::calculateIOU(slices[i], slices[j]);
			}

			if (iou > best_iou) {
				best_iou = iou;
				best_slice = i;
			}
		}
		
		// extract contours in the specified region
		std::vector<util::Polygon> polygons = util::findContours(slices[best_slice]);
		if (polygons.size() == 0) throw "No building is found.";

		return simplifyPolygon(polygons[0], epsilon);
	}

	util::Polygon OpenCVSimplification::simplifyPolygon(const util::Polygon& polygon, double epsilon) {
		util::Polygon ans;
		cv::approxPolyDP(polygon.contour, ans.contour, epsilon, true);
		if (ans.contour.size() < 3) ans.contour = polygon.contour;
	
		// simplify the hole as well
		for (int i = 0; i < polygon.holes.size(); i++) {
			std::vector<cv::Point2f> simplified_hole;
			cv::approxPolyDP(polygon.holes[i], simplified_hole, epsilon, true);
			if (simplified_hole.size() >= 3) {
				ans.holes.push_back(simplified_hole);
			}
		}

		// ToDo:
		// Should we check if the holes are inside the contour?

		return ans;
	}

}