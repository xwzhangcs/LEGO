#include "DisjointVoxelData.h"
#include "ContourUtils.h"

namespace util {

	DisjointVoxelData::DisjointVoxelData() {
	}
	
	std::vector<std::shared_ptr<BuildingLayer>> DisjointVoxelData::disjoint(const std::vector<cv::Mat_<uchar>>& voxel_data) {
		std::vector<std::shared_ptr<BuildingLayer>> buildings;
		
		std::vector<std::shared_ptr<BuildingLayer>> layers;
		cv::Mat_<unsigned short> cluster = cv::Mat_<unsigned short>::zeros(voxel_data[0].size());

		printf("Processing slice");
		for (int h = 0; h < voxel_data.size(); h++) {
			printf("\rProcessing slice %d  ", h);
			int next_cluster_id = 1;
			cv::Mat_<unsigned short> next_cluster = cv::Mat_<unsigned short>::zeros(cluster.size());
			std::vector<std::shared_ptr<BuildingLayer>> next_layers;

			for (int r = 0; r < voxel_data[h].rows; r++) {
				for (int c = 0; c < voxel_data[h].cols; c++) {
					if (voxel_data[h](r, c) == 255 && cluster(r, c) > 0 && next_cluster(r, c) == 0) {
						int cluster_id = cluster(r, c);
						traverseInSlice(voxel_data, next_cluster, next_cluster_id, h, r, c);

						// extract contour
						cv::Mat_<uchar> slice = getSliceOfCluster(voxel_data, h, next_cluster, next_cluster_id);
						std::vector<Polygon> polygons = findContours(slice, false);

						// convert the polygon coordinates to the world coordinate system
						for (int i = 0; i < polygons[0].contour.size(); i++) {
							polygons[0].contour[i].x = polygons[0].contour[i].x - voxel_data[0].cols / 2;
							polygons[0].contour[i].y = voxel_data[0].rows / 2 - polygons[0].contour[i].y;
						}
						for (int i = 0; i < polygons[0].holes.size(); i++) {
							for (int j = 0; j < polygons[0].holes[i].size(); j++) {
								polygons[0].holes[i][j].x = polygons[0].holes[i][j].x - voxel_data[0].cols / 2;
								polygons[0].holes[i][j].y = voxel_data[0].rows / 2 - polygons[0].holes[i][j].y;
							}
						}

						// Create the next building layer
						std::shared_ptr<BuildingLayer> next_layer = std::shared_ptr<BuildingLayer>(new BuildingLayer(layers[cluster_id - 1]->building_id, h, h + 1));
						next_layer->raw_footprints.push_back(polygons[0]);
						next_layer->footprint = polygons[0];
						layers[cluster_id - 1]->children.push_back(next_layer);

						next_layers.push_back(next_layer);

						next_cluster_id++;
					}
				}
			}

			// If there is a voxel that has not been visited, it should belong to the bottom layer.
			for (int r = 0; r < voxel_data[h].rows; r++) {
				for (int c = 0; c < voxel_data[h].cols; c++) {
					if (voxel_data[h](r, c) == 255 && next_cluster(r, c) == 0) {
						traverseInSlice(voxel_data, next_cluster, next_cluster_id, h, r, c);

						// extract contour
						cv::Mat_<uchar> slice = getSliceOfCluster(voxel_data, h, next_cluster, next_cluster_id);
						std::vector<Polygon> polygons = findContours(slice, false);

						// convert the polygon coordinates to the world coordinate system
						for (int i = 0; i < polygons[0].contour.size(); i++) {
							polygons[0].contour[i].x = polygons[0].contour[i].x - voxel_data[0].cols / 2;
							polygons[0].contour[i].y = voxel_data[0].rows / 2 - polygons[0].contour[i].y;
						}
						for (int i = 0; i < polygons[0].holes.size(); i++) {
							for (int j = 0; j < polygons[0].holes[i].size(); j++) {
								polygons[0].holes[i][j].x = polygons[0].holes[i][j].x - voxel_data[0].cols / 2;
								polygons[0].holes[i][j].y = voxel_data[0].rows / 2 - polygons[0].holes[i][j].y;
							}
						}

						// Create the bottom layer of the building.
						std::shared_ptr<BuildingLayer> building = std::shared_ptr<BuildingLayer>(new BuildingLayer(buildings.size(), h, h + 1));
						building->raw_footprints.push_back(polygons[0]);
						building->footprint = polygons[0];
						buildings.push_back(building);

						next_layers.push_back(building);

						next_cluster_id++;
					}
				}
			}

			layers = next_layers;
			cluster = next_cluster;
		}
		printf("\n");

		return buildings;
	}

	std::shared_ptr<BuildingLayer> DisjointVoxelData::layering(const std::shared_ptr<BuildingLayer>& building, float threshold) {
		std::shared_ptr<BuildingLayer> bottom_building_layer = std::shared_ptr<BuildingLayer>(new BuildingLayer(building->building_id, building->bottom_height, building->top_height));
		bottom_building_layer->raw_footprints = building->raw_footprints;
		bottom_building_layer->footprint = building->footprint;

		if (building->children.size() == 0) return bottom_building_layer;

		std::queue<std::pair<std::shared_ptr<BuildingLayer>, std::shared_ptr<BuildingLayer>>> Q;
		Q.push({ building, bottom_building_layer });
		while (!Q.empty()) {
			std::shared_ptr<BuildingLayer> parent = Q.front().first;
			std::shared_ptr<BuildingLayer> new_parent = Q.front().second;
			Q.pop();

			// remove too small layers
			float parent_area = calculateArea(parent->raw_footprints.front());
			for (int i = (int)parent->children.size() - 1; i >= 0; i--) {
				if (parent->children[i]->children.size() == 0 && calculateArea(parent->children[i]->raw_footprints.front()) / parent_area < 0.1) parent->children.erase(parent->children.begin() + i);
			}

			if (parent->children.size() == 1 && calculateIOU(parent->raw_footprints.front(), parent->children[0]->raw_footprints.front()) >= threshold) {
				new_parent->raw_footprints.push_back(parent->children[0]->raw_footprints.front());
				new_parent->top_height = parent->children[0]->top_height;

				Q.push({ parent->children[0], new_parent });
			}
			else {
				for (int i = 0; i < parent->children.size(); i++) {
					std::shared_ptr<BuildingLayer> new_building_layer = std::shared_ptr<BuildingLayer>(new BuildingLayer(parent->children[i]->building_id, parent->children[i]->bottom_height, parent->children[i]->top_height));
					new_building_layer->raw_footprints = parent->children[i]->raw_footprints;
					new_building_layer->footprint = parent->children[i]->footprint;
					new_parent->children.push_back(new_building_layer);

					Q.push({ parent->children[i], new_building_layer });
				}
			}
		}

		return bottom_building_layer;
	}
	
	cv::Mat_<uchar> DisjointVoxelData::getSliceOfCluster(const std::vector<cv::Mat_<uchar>>& voxel_data, int slice_id, const cv::Mat_<unsigned short>& cluster_data, int cluster_id) {
		cv::Mat_<uchar> ans = cv::Mat_<uchar>::zeros(voxel_data[slice_id].size());
		for (int r = 0; r < voxel_data[slice_id].rows; r++) {
			for (int c = 0; c < voxel_data[slice_id].cols; c++) {
				if (voxel_data[slice_id](r, c) == 255 && cluster_data(r, c) == cluster_id) {
					ans(r, c) = 255;
				}
			}
		}
		return ans;
	}

	/**
	 * Traverse the pixels in the slice with value of 255 and are not visited yet.
	 *
	 * @param cluster		save the clustering information
	 * @param cluster_id	current cluster id
	 * @param slice			slice image
	 * @param r				row of current pixel
	 * @param c				column of current pixel
	 */
	void DisjointVoxelData::traverseInSlice(const std::vector<cv::Mat_<uchar>>& voxel_data, cv::Mat_<unsigned short>& cluster, int cluster_id, int slice_id, int r, int c) {
		std::queue<std::pair<int, int>> Q;
		Q.push(std::make_pair(r, c));

		while (!Q.empty()) {
			std::pair<int, int> t = Q.front();
			Q.pop();
			int r = t.first;
			int c = t.second;

			if (voxel_data[slice_id](r, c) == 255 && cluster(r, c) == 0) {
				cluster(r, c) = cluster_id;

				if (r > 0) {
					Q.push(std::make_pair(r - 1, c));
				}
				if (r < voxel_data[slice_id].rows - 1) {
					Q.push(std::make_pair(r + 1, c));
				}
				if (c > 0) {
					Q.push(std::make_pair(r, c - 1));
				}
				if (c < voxel_data[slice_id].cols - 1) {
					Q.push(std::make_pair(r, c + 1));
				}
			}
		}
	}
	
}