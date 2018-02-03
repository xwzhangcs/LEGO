#include "DisjointVoxelData.h"

namespace util {

	std::vector<std::vector<cv::Mat_<uchar>>> DisjointVoxelData::disjoint(const std::vector<cv::Mat_<uchar>>& voxel_data, float threshold) {
		// initialize clustering data
		std::vector<cv::Mat_<int>> clustered_voxel_data(voxel_data.size());
		for (int i = 0; i < voxel_data.size(); i++) {
			clustered_voxel_data[i] = cv::Mat_<int>::zeros(voxel_data[i].size());
		}

		int cluster_id = 1;
		for (int slice_id = 0; slice_id < voxel_data.size(); slice_id++) {
			for (int r = 0; r < voxel_data[slice_id].rows; r++) {
				for (int c = 0; c < voxel_data[slice_id].cols; c++) {
					if (voxel_data[slice_id](r, c) >= 255 * threshold && clustered_voxel_data[slice_id](r, c) == 0) {
						traverse(voxel_data, threshold, clustered_voxel_data, cluster_id, slice_id, r, c);
						cluster_id++;
					}
				}
			}
		}

		std::vector<std::vector<cv::Mat_<uchar>>> ans(cluster_id - 1);
		for (int i = 0; i < ans.size(); i++) {
			ans[i].resize(voxel_data.size());
			for (int slice_id = 0; slice_id < voxel_data.size(); slice_id++) {
				ans[i][slice_id] = cv::Mat(voxel_data[slice_id].size(), CV_8U, cv::Scalar(0));
				for (int r = 0; r < clustered_voxel_data[slice_id].rows; r++) {
					for (int c = 0; c < clustered_voxel_data[slice_id].cols; c++) {
						if (clustered_voxel_data[slice_id](r, c) == i + 1) {
							ans[i][slice_id](r, c) = voxel_data[slice_id](r, c);
						}
					}
				}
			}
		}

		return ans;
	}

	void DisjointVoxelData::traverse(const std::vector<cv::Mat_<uchar>>& voxel_data, float threshold, std::vector<cv::Mat_<int>>& clustered_voxel_data, int cluster_id, int slice_id, int r, int c) {
		std::queue<std::tuple<int, int, int>> Q;
		Q.push(std::make_tuple(slice_id, r, c));

		while (!Q.empty()) {
			std::tuple<int, int, int> t = Q.front();
			Q.pop();
			int slice_id = std::get<0>(t);
			int r = std::get<1>(t);
			int c = std::get<2>(t);

			if (voxel_data[slice_id].at<uchar>(r, c) >= 255 * threshold && clustered_voxel_data[slice_id](r, c) == 0) {
				clustered_voxel_data[slice_id](r, c) = cluster_id;

				if (r > 0) {
					Q.push(std::make_tuple(slice_id, r - 1, c));
				}
				if (r < voxel_data[slice_id].rows - 1) {
					Q.push(std::make_tuple(slice_id, r + 1, c));
				}
				if (c > 0) {
					Q.push(std::make_tuple(slice_id, r, c - 1));
				}
				if (c < voxel_data[slice_id].cols - 1) {
					Q.push(std::make_tuple(slice_id, r, c + 1));
				}
				if (slice_id < voxel_data.size() - 1) {
					Q.push(std::make_tuple(slice_id + 1, r, c));
				}
			}
		}
	}

}