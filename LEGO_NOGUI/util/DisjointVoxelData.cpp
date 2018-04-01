#include "DisjointVoxelData.h"
#include "ContourUtils.h"

namespace util {

	DisjointVoxelData::DisjointVoxelData() {
	}
	
	std::vector<std::shared_ptr<BuildingLayer>> DisjointVoxelData::disjoint(const std::vector<cv::Mat_<uchar>>& voxel_data) {
		// initialize the clustering
		std::vector<cv::Mat_<unsigned short>> clustering(voxel_data.size());
		for (int h = 0; h < voxel_data.size(); h++) {
			clustering[h] = cv::Mat_<unsigned short>::zeros(voxel_data[h].size());
		}

		// perform 3D dfs to cluster the voxel data
		setbuf(stdout, NULL);
		printf("Clustering the voxel data...\n");
		int cluster_id = 1;
		for (int h = 0; h < voxel_data.size(); h++) {
			for (int r = 0; r < voxel_data[h].rows; r++) {
				for (int c = 0; c < voxel_data[h].cols; c++) {
					if (voxel_data[h](r, c) == 255 && clustering[h](r, c) == 0) {
						dfs3D(voxel_data, clustering, r, c, h, cluster_id);
						cluster_id++;
					}
				}
			}
		}

		int num_cluster_id = cluster_id - 1;

		std::vector<std::shared_ptr<BuildingLayer>> buildings;
		std::vector<std::shared_ptr<BuildingLayer>> layers;

		printf("Processing slice");
		for (int h = 0; h < voxel_data.size(); h++) {
			printf("\rProcessing slice %d  ", h + 1);

			for (int cluster_id = 1; cluster_id <= num_cluster_id; cluster_id++) {
				try {
					// extract contour
					int min_x, min_y, max_x, max_y;
					cv::Mat_<uchar> slice = getSliceOfCluster(clustering[h], cluster_id, min_x, min_y, max_x, max_y);
					cv::Mat_<uchar> roi_slice(slice, cv::Rect(min_x, min_y, max_x - min_x + 1, max_y - min_y + 1));
					std::vector<Polygon> polygons = findContours(roi_slice, false);
					for (int i = 0; i < polygons.size(); i++) {
						polygons[i].translate(min_x, min_y);
					}

					// convert the polygon coordinates to the world coordinate system
					for (auto& polygon : polygons) {
						for (int i = 0; i < polygon.contour.size(); i++) {
							polygon.contour[i].x = polygon.contour[i].x - voxel_data[0].cols / 2;
							polygon.contour[i].y = voxel_data[0].rows / 2 - polygon.contour[i].y;
						}
						for (auto& hole : polygon.holes) {
							for (int j = 0; j < hole.size(); j++) {
								hole[j].x = hole[j].x - voxel_data[0].cols / 2;
								hole[j].y = voxel_data[0].rows / 2 - hole[j].y;
							}
						}
					}

					// Create the next building layer
					int building_id;
					if (cluster_id - 1 >= buildings.size()) {
						building_id = buildings.size();
						layers.resize(layers.size() + 1);
						layers[cluster_id - 1] = std::shared_ptr<BuildingLayer>(new BuildingLayer(building_id, h, h + 1));
						layers[cluster_id - 1]->raw_footprints.push_back(polygons);
						layers[cluster_id - 1]->footprints = polygons;
						buildings.push_back(layers[cluster_id - 1]);
					}
					else {
						building_id = layers[cluster_id - 1]->building_id;

						layers[cluster_id - 1]->child = std::shared_ptr<BuildingLayer>(new BuildingLayer(building_id, h, h + 1));
						layers[cluster_id - 1] = layers[cluster_id - 1]->child;
						layers[cluster_id - 1]->raw_footprints.push_back(polygons);
						layers[cluster_id - 1]->footprints = polygons;
					}
				}
				catch (...) {
				}

			}
		}
		printf("\n");

		return buildings;
	}

	std::shared_ptr<BuildingLayer> DisjointVoxelData::layering(const std::shared_ptr<BuildingLayer>& building, float threshold) {
		std::shared_ptr<BuildingLayer> bottom_building_layer = std::shared_ptr<BuildingLayer>(new BuildingLayer(building->building_id, building->footprints, building->bottom_height, building->top_height));
		bottom_building_layer->raw_footprints = building->raw_footprints;

		std::shared_ptr<BuildingLayer> cur = bottom_building_layer;
		std::shared_ptr<BuildingLayer> child = building->child;
		while (child) {
			if (calculateIOU(cur->raw_footprints[0], child->raw_footprints[0]) >= threshold) {
				cur->raw_footprints.push_back(child->raw_footprints[0]);
				cur->top_height = child->top_height;
			}
			else {
				cur->child = std::shared_ptr<BuildingLayer>(new BuildingLayer(child->building_id, child->footprints, child->bottom_height, child->top_height));
				cur = cur->child;
				cur->raw_footprints = child->raw_footprints;
			}

			child = child->child;
		}
		if (cur) cur->child.reset();

		// Recursively remove too small layer if it has no child layer.
		removeSmallLayers(bottom_building_layer);
		
		return bottom_building_layer;
	}

	/**
	 * Traverse the voxel data with value of 255 and are not visited yet.
	 *
	 * @param voxel_data	voxel data
	 * @param clustering	save the clustering information
	 * @param r				row of current voxel
	 * @param c				column of current voxel
	 * @param h				height of current voxel
	 * @param cluster_id	current cluster id
	 */
	void DisjointVoxelData::dfs3D(const std::vector<cv::Mat_<uchar>>&voxel_data, std::vector<cv::Mat_<unsigned short>>&clustering, int r, int c, int h, int cluster_id) {
		const std::vector<std::pair<int, std::pair<int, int>>> dirs = { { 0, { 1, 0 } }, { 0, { -1, 0 } }, { 0, { 0, 1 } }, { 0, { 0, -1 } }, { 1, { 0, 0 } }, { -1, { 0, 0 } } };

		int R = voxel_data[0].rows;
		int C = voxel_data[0].cols;

		std::queue<std::pair<int, std::pair<int, int>>> Q;
		Q.push({ h, { r, c } });

		clustering[h](r, c) = cluster_id;

		while (!Q.empty()) {
			int h = Q.front().first;
			int r = Q.front().second.first;
			int c = Q.front().second.second;
			Q.pop();

			for (int i = 0; i < dirs.size(); i++) {
				int h2 = h + dirs[i].first;
				int r2 = r + dirs[i].second.first;
				int c2 = c + dirs[i].second.second;

				if (r2 >= 0 && r2 < R && c2 >= 0 && c2 < C && h2 >= 0 && h2 < voxel_data.size() && voxel_data[h2](r2, c2) == 255 && clustering[h2](r2, c2) == 0) {
					clustering[h2](r2, c2) = cluster_id;
					Q.push({ h2, { r2, c2 } });
				}
			}
		}
	}

	cv::Mat_<uchar> DisjointVoxelData::getSliceOfCluster(const cv::Mat_<unsigned short>& clustering, int cluster_id, int& min_x, int& min_y, int& max_x, int& max_y) {
		min_x = std::numeric_limits<int>::max();
		min_y = std::numeric_limits<int>::max();
		max_x = 0;
		max_y = 0;

		bool found = false;

		cv::Mat_<uchar> ans = cv::Mat_<uchar>::zeros(clustering.size());
		for (int r = 0; r < clustering.rows; r++) {
			for (int c = 0; c < clustering.cols; c++) {
				if (clustering(r, c) == cluster_id) {
					ans(r, c) = 255;
					min_x = std::min(min_x, c);
					min_y = std::min(min_y, r);
					max_x = std::max(max_x, c);
					max_y = std::max(max_y, r);
					found = true;
				}
			}
		}

		if (!found) throw "No specified cluster was found.";

		return ans;
	}

	void DisjointVoxelData::removeSmallLayers(std::shared_ptr<BuildingLayer> layer) {
		while (layer) {
			for (int i = layer->footprints.size() - 1; i >= 0; i--) {
				if (util::calculateArea(layer->footprints[i]) < 5) layer->footprints.erase(layer->footprints.begin() + i);
			}
			layer = layer->child;
		} 
	}

}