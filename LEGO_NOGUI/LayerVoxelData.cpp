#include "LayerVoxelData.h"
#include "ContourUtils.h"

namespace util {

	/**
	 * Select the representative slice that has the best IOU with all the slices in the layer.
	 */
	cv::Mat_<uchar> Layer::selectRepresentativeSlice() {
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

		return slices[best_slice];
	}

	LayerVoxelData::LayerVoxelData(const std::vector<cv::Mat_<uchar>>& voxel_data, float threshold) {
		this->voxel_data = voxel_data;
		this->threshold = 255 * threshold;
	}

	std::shared_ptr<Layer> LayerVoxelData::layering(float layering_threshold) {
		int height = 5;

		int voxel_count = countVoxel(voxel_data[height]);
		if (voxel_count == 0) throw "No voxel at the bottom.";

		std::shared_ptr<Layer> layer = std::shared_ptr<Layer>(new Layer(height, height));
		layering(layer, voxel_data[height], height, voxel_data[height], layering_threshold);
		return layer;
	}

	/**
	 * Grow layering to the upper layers.
	 *
	 * @param current_layer		current layer
	 * @param current slice		current slice image
	 * @param height			current height
	 */
	void LayerVoxelData::layering(std::shared_ptr<Layer> current_layer, const cv::Mat_<uchar>& current_slice, int height, const cv::Mat_<uchar>& bottom_slice, float layering_threshold) {
		cv::Mat_<int> cluster = cv::Mat_<int>::zeros(current_slice.size());
		current_layer->slices.push_back(current_slice);
		current_layer->top_height = height + 1;

		if (height >= voxel_data.size() - 1) return;

		std::vector<cv::Mat_<uchar>> next_slices;

		int cluster_id = 1;
		for (int r = 0; r < voxel_data[height + 1].rows; r++) {
			for (int c = 0; c < voxel_data[height + 1].cols; c++) {
				if (current_slice(r, c) >= threshold && voxel_data[height + 1](r, c) >= threshold && cluster(r, c) == 0) {
					traverse(cluster, cluster_id, voxel_data[height + 1], r, c);

					// create the next slice
					next_slices.resize(next_slices.size() + 1);
					createSlice(voxel_data[height + 1], cluster, cluster_id, next_slices.back());

					cluster_id++;
				}
			}
		}
				
		if (next_slices.size() == 0) return;

		float iou = util::calculateIOU(next_slices[0], bottom_slice);
		if (next_slices.size() > 1 || iou < layering_threshold) {
			for (int i = 0; i < next_slices.size(); i++) {
				std::shared_ptr<Layer> next_layer = std::shared_ptr<Layer>(new Layer(height + 1, height + 1));
				current_layer->children.push_back(next_layer);
				layering(next_layer, next_slices[i], height + 1, next_slices[i], layering_threshold);
			}
		}
		else {
			layering(current_layer, next_slices[0], height + 1, bottom_slice, layering_threshold);
		}
	}

	/**
	 * Create a slice of the specified cluster id based on the cluster data.
	 *
	 * @param slice			original slice image
	 * @param cluster		cluster data
	 * @param cluster_id	specified cluster id
	 * @param slice			output slice image
	 */
	void LayerVoxelData::createSlice(const cv::Mat_<uchar>& original_slice, cv::Mat_<int> cluster, int cluster_id, cv::Mat_<uchar>& slice) {
		slice = cv::Mat_<uchar>::zeros(original_slice.size());

		for (int r = 0; r < original_slice.rows; r++) {
			for (int c = 0; c < original_slice.cols; c++) {
				if (cluster(r, c) == cluster_id) {
					slice(r, c) = original_slice(r, c);
				}
			}
		}
	}

	/**
	 * Traverse the pixels of the slice that have values over a threshold and are not visited yet.
	 *
	 * @param cluster		save the clustering information
	 * @param cluster_id	current cluster id
	 * @param slice			slice image
	 * @param r				row of current pixel
	 * @param c				column of current pixel
	 */
	void LayerVoxelData::traverse(cv::Mat_<int>& cluster, int cluster_id, const cv::Mat_<uchar>& slice, int r, int c) {
		std::queue<std::pair<int, int>> Q;
		Q.push(std::make_pair(r, c));

		while (!Q.empty()) {
			std::pair<int, int> t = Q.front();
			Q.pop();
			int r = t.first;
			int c = t.second;

			if (slice(r, c) >= threshold && cluster(r, c) == 0) {
				cluster(r, c) = cluster_id;

				if (r > 0) {
					Q.push(std::make_pair(r - 1, c));
				}
				if (r < voxel_data[cluster_id].rows - 1) {
					Q.push(std::make_pair(r + 1, c));
				}
				if (c > 0) {
					Q.push(std::make_pair(r, c - 1));
				}
				if (c < voxel_data[cluster_id].cols - 1) {
					Q.push(std::make_pair(r, c + 1));
				}
			}
		}
	}

	int LayerVoxelData::countVoxel(const cv::Mat_<uchar>& slice) {
		int voxel_cnt = 0;
		for (int r = 0; r < slice.rows; r++) {
			for (int c = 0; c < slice.cols; c++) {
				voxel_cnt += slice(r, c);
			}
		}
		return voxel_cnt;
	}
	
}
