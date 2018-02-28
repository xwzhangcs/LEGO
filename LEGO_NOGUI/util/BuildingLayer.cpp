#include "BuildingLayer.h"

namespace util {

	BuildingLayer::BuildingLayer(int building_id, float bottom_height, float top_height) {
		this->building_id = building_id;
		this->bottom_height = bottom_height;
		this->top_height = top_height;
	}

	BuildingLayer::BuildingLayer(int building_id, const util::Polygon& footprint, float bottom_height, float top_height) {
		this->building_id = building_id;
		this->footprint = footprint;
		this->bottom_height = bottom_height;
		this->top_height = top_height;
	}

	util::Polygon BuildingLayer::selectRepresentativeContour() const {
		double best_iou = 0;
		int best_slice = -1;
		for (int i = 0; i < raw_footprints.size(); i++) {
			double iou = 0;
			for (int j = 0; j < raw_footprints.size(); j++) {
				// calculate IOU
				iou += util::calculateIOU(raw_footprints[i], raw_footprints[j]);
			}

			if (iou > best_iou) {
				best_iou = iou;
				best_slice = i;
			}
		}

		return raw_footprints[best_slice];
	}

}