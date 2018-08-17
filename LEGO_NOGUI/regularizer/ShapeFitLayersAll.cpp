#include "ShapeFitLayersAll.h"
#include <QFileDialog>
#include <QTextStream>
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"

ShapeFitLayersAll::ShapeFitLayersAll() {
}

ShapeFitLayersAll::~ShapeFitLayersAll() {
}

void ShapeFitLayersAll::fit(std::vector<Layer>& layers, QString config_file)
{
	bool bUseIntra = false;
	float intraWeight = 0.0f;
	bool bUseInter = false;
	float interWeight = 0.0f;
	bool bUseRaOpt = false;
	float angle_threshold_RA = 0.0f;
	float raWeight = 0.0f;
	bool bUseParallelOpt = false;
	float angle_threshold_parallel = 0.0f;
	float parallelWeight = 0.0f;
	bool bUseSymmetryLineOpt = false;
	float symmetryWeight = 0.0f;
	bool bUseAccuracyOpt = false;
	float accuracyWeight = 0.0f;
	bool bUsePointSnapOpt = false;
	float pointDisThreshold = 0.0f;
	float pointWeight = 0.0f;
	bool bUseSegSnapOpt = false;
	float segDisThreshold = 0.0f;
	float segAngleThreshold = 0.0f;
	float segWeight = 0.0f;

	QFile file(config_file);
	if (!file.open(QIODevice::ReadOnly)) {
		std::cerr << "File was not readable: " << std::endl;
		return;
	}
	QTextStream in(&file);
	rapidjson::Document doc;
	doc.Parse(in.readAll().toUtf8().constData());
	bUseIntra = doc["UseIntra"].GetBool();
	bUseInter = doc["UseInter"].GetBool();
	intraWeight = doc["Intra_Weight"].GetFloat();
	interWeight = doc["Inter_Weight"].GetFloat();
	if (bUseIntra){
		rapidjson::Value& algs = doc["IntraOpt"];
		//ra
		rapidjson::Value& algs_ra = algs["RA"];
		bUseRaOpt = algs_ra["UseOpt"].GetBool();
		angle_threshold_RA = algs_ra["AngleThreshold"].GetFloat();
		raWeight = algs_ra["Weight"].GetFloat();
		//symmetry
		rapidjson::Value& algs_symmetry = algs["Symmetry"];
		bUseSymmetryLineOpt = algs_symmetry["UseOpt"].GetBool();
		symmetryWeight = algs_symmetry["Weight"].GetFloat();
		// parallel
		rapidjson::Value& algs_parallel = algs["Parallel"];
		bUseParallelOpt = algs_parallel["UseOpt"].GetBool();
		angle_threshold_parallel = algs_parallel["AngleThreshold"].GetFloat();
		parallelWeight = algs_parallel["Weight"].GetFloat();
		// accuracy
		rapidjson::Value& algs_accuracy = algs["Accuracy"];
		bUseAccuracyOpt = algs_accuracy["UseOpt"].GetBool();
		accuracyWeight = algs_accuracy["Weight"].GetFloat();
	}
	if (bUseInter){
		rapidjson::Value& algs = doc["InterOpt"];
		// point snap
		rapidjson::Value& algs_point = algs["PointSnap"];
		bUsePointSnapOpt = algs_point["UseOpt"].GetBool();
		pointDisThreshold = algs_point["DisThreshold"].GetFloat();
		pointWeight = algs_point["Weight"].GetFloat();
		// seg snap
		rapidjson::Value& algs_seg = algs["SegSnap"];
		bUseSegSnapOpt = algs_seg["UseOpt"].GetBool();
		segDisThreshold = algs_seg["DisThreshold"].GetFloat();
		segAngleThreshold = algs_seg["AngleThreshold"].GetFloat();
		segWeight = algs_seg["Weight"].GetFloat();
	}
	file.close();
	if (!bUseInter && !bUseIntra){
		return;
	}
	//check weights
	if (bUseInter || bUseIntra){
		float weight = 0.0f;
		if (bUseInter)
			weight += interWeight;
		if (bUseIntra)
			weight += intraWeight;
		if (abs(weight - 1.0f) < 0.0001)
		{
			//
		}
		else{
			std::cout << "Please check intra and inter weight assignment!!!" << std::endl;
			return;
		}
	}
	if (bUseIntra){
		//check the sum weight equals 1
		float weight = 0.0f;
		if (bUseRaOpt)
			weight += raWeight;
		if (bUseParallelOpt)
			weight += parallelWeight;
		if (bUseSymmetryLineOpt)
			weight += symmetryWeight;
		if (bUseAccuracyOpt)
			weight += accuracyWeight;
		if (abs(weight - 1.0f) < 0.0001)
		{
			//
		}
		else{
			std::cout << "Please check intra weight assignment!!!" << std::endl;
			return;
		}
	}
	if (bUseInter){
		float weight = 0.0f;
		if (bUsePointSnapOpt)
			weight += pointWeight;
		if (bUseSegSnapOpt)
			weight += segWeight;
		if (abs(weight - 1.0f) < 0.0001)
		{
			//
		}
		else{
			std::cout << "Please check inter weight assignment!!!" << std::endl;
			return;
		}
	}

	float min_x = std::numeric_limits<float>::max();
	float min_y = std::numeric_limits<float>::max();
	float max_x = -std::numeric_limits<float>::max();
	float max_y = -std::numeric_limits<float>::max();
	for (int k = 0; k < layers.size(); k++){
		for (int i = 0; i < layers[k].sparse_contours.size(); i++) {
			for (int j = 0; j < layers[k].sparse_contours[i].size(); j++){
				min_x = std::min(min_x, layers[k].sparse_contours[i][j].x);
				min_y = std::min(min_y, layers[k].sparse_contours[i][j].y);
				max_x = std::max(max_x, layers[k].sparse_contours[i][j].x);
				max_y = std::max(max_y, layers[k].sparse_contours[i][j].y);
			}
		}
	}
	for (int k = 0; k < layers.size(); k++){
		for (int i = 0; i < layers[k].contours.size(); i++) {
			for (int j = 0; j < layers[k].contours[i].size(); j++){
				min_x = std::min(min_x, layers[k].contours[i][j].x);
				min_y = std::min(min_y, layers[k].contours[i][j].y);
				max_x = std::max(max_x, layers[k].contours[i][j].x);
				max_y = std::max(max_y, layers[k].contours[i][j].y);
			}
		}
	}
	if (bUseSymmetryLineOpt){
		for (int k = 0; k < layers.size(); k++){
			for (int i = 0; i < layers[k].symmetry_lines.size(); i++){
				for (int j = 0; j < layers[k].symmetry_lines[i].size(); j++){
					min_x = std::min(min_x, layers[k].symmetry_lines[i][j].x);
					min_y = std::min(min_y, layers[k].symmetry_lines[i][j].y);
					max_x = std::max(max_x, layers[k].symmetry_lines[i][j].x);
					max_y = std::max(max_y, layers[k].symmetry_lines[i][j].y);
				}
			}
		}
	}
	float max_unit = std::max(max_x - min_x + 1.0f, max_y - min_y + 1.0f);
	Config config(bUseIntra, intraWeight, bUseInter, interWeight, bUseRaOpt, angle_threshold_RA, raWeight, bUseParallelOpt, angle_threshold_parallel, parallelWeight, bUseSymmetryLineOpt, symmetryWeight, bUseAccuracyOpt, accuracyWeight, bUsePointSnapOpt, pointDisThreshold / max_unit, pointWeight, bUseSegSnapOpt, segDisThreshold / max_unit, segAngleThreshold, segWeight);
	//Config config(bUseIntra, intraWeight, bUseInter, interWeight, bUseRaOpt, angle_threshold_RA, raWeight, bUseParallelOpt, angle_threshold_parallel, parallelWeight, bUseSymmetryLineOpt, symmetryWeight, bUseAccuracyOpt, accuracyWeight, bUsePointSnapOpt, pointDisThreshold, pointWeight, bUseSegSnapOpt, segDisThreshold, segAngleThreshold, segWeight);

	std::vector<layer_polygons> normalized_polygons(layers.size());
	for (int k = 0; k < layers.size(); k++){
		normalized_polygons[k].resize(layers[k].sparse_contours.size());
		for (int i = 0; i < layers[k].sparse_contours.size(); i++) {
			normalized_polygons[k][i].resize(layers[k].sparse_contours[i].size());
			for (int j = 0; j < layers[k].sparse_contours[i].size(); j++){
				normalized_polygons[k][i][j] = cv::Point2f((layers[k].sparse_contours[i][j].x - min_x) / max_unit, (layers[k].sparse_contours[i][j].y - min_y) / max_unit);
			}
		}
	}

	// normalized_symmetry_line
	std::vector<layer_polygons> normalized_symmetry_lines;
	if (bUseSymmetryLineOpt){
		normalized_symmetry_lines.resize(layers.size());
		for (int k = 0; k < layers.size(); k++){
			normalized_symmetry_lines[k].resize(layers[k].symmetry_lines.size());
			for (int i = 0; i < layers[k].symmetry_lines.size(); i++) {
				normalized_symmetry_lines[k][i].resize(layers[k].symmetry_lines[i].size());
				for (int j = 0; j < layers[k].symmetry_lines[i].size(); j++){
					normalized_symmetry_lines[k][i][j] = cv::Point2f((layers[k].symmetry_lines[i][j].x - min_x) / max_unit, (layers[k].symmetry_lines[i][j].y - min_y) / max_unit);
				}
			}
		}
	}
	int total_points = 0;
	std::vector<layer_polygons> normalized_polygons_init(layers.size());
	for (int k = 0; k < layers.size(); k++){
		normalized_polygons_init[k].resize(layers[k].contours.size());
		for (int i = 0; i < layers[k].contours.size(); i++) {
			normalized_polygons_init[k][i].resize(layers[k].contours[i].size());
			total_points += layers[k].contours[i].size();
			for (int j = 0; j < layers[k].contours[i].size(); j++){
				normalized_polygons_init[k][i][j] = cv::Point2f((layers[k].contours[i][j].x - min_x) / max_unit, (layers[k].contours[i][j].y - min_y) / max_unit);
			}
		}
	}
	std::vector<std::pair<float, float>> layers_height;
	layers_height.resize(layers.size());
	for (int k = 0; k < layers.size(); k++){
		layers_height[k] = std::make_pair(layers[k].top_height, layers[k].bottom_height);
	}
	// tree info
	std::vector<std::pair<std::vector<int>, std::vector<int>>> tree_info;
	tree_info.resize(layers.size());
	for (int k = 0; k < layers.size(); k++){
		tree_info[k] = std::make_pair(layers[k].parents, layers[k].children);
	}
	// initial
	std::vector<std::vector<bool>> validity_layers;
	validity_layers.resize(layers.size());
	for (int i = 0; i < layers.size(); i++){
		validity_layers[i].push_back(false);// RA
		validity_layers[i].push_back(false);// Parallel
		validity_layers[i].push_back(false);// Symmetry
		validity_layers[i].push_back(false);// Accuracy
		validity_layers[i].push_back(false);// Point
		validity_layers[i].push_back(false);// Seg
	}

	// check validity of RA
	if (bUseRaOpt){
		bool bValid_per_layer = false;
		bool bValid = false;
		for (int k = 0; k < layers.size(); k++){
			bValid_per_layer = false;
			for (int i = 0; i < layers[k].contours.size(); i++){
				if (layers[k].contours[i].size() != 0){
					if (validRA(layers[k].contours[i], bUseRaOpt, angle_threshold_RA)){
						bValid_per_layer = true;
						break;
					}
				}
			}
			if (bValid_per_layer){
				layers[k].bUseRaOptValid = true;
				validity_layers[k][0] = true;
				bValid = true;
			}
			else{
				validity_layers[k][0] = false;
				layers[k].bUseRaOptValid = false;
			}
		}
		if (!bValid){
			config.bUseRaOpt = false;
			std::cout << "RA invalid" << std::endl;
		}
	}
	// check validity of Parallel
	if (bUseParallelOpt){
		bool bValid_per_layer = false;
		bool bValid = false;
		for (int k = 0; k < layers.size(); k++){
			bValid_per_layer = false;
			for (int i = 0; i < layers[k].contours.size(); i++){
				if (layers[k].contours[i].size() != 0){
					if (validParallel(layers[k].contours[i], bUseParallelOpt, angle_threshold_parallel)){
						bValid_per_layer = true;
						break;
					}
				}
			}
			if (bValid_per_layer){
				validity_layers[k][1] = true;
				layers[k].bUseParallelOptValid = true;
				bValid = true;
			}
			else{
				validity_layers[k][1] = false;
				layers[k].bUseParallelOptValid = false;
			}
		}
		if (!bValid){
			config.bUseParallelOpt = false;
			std::cout << "Parallel invalid" << std::endl;
		}
	}
	// check validity of symmetry
	if (bUseSymmetryLineOpt){
		bool bValid_per_layer = false;
		bool bValid = false;
		for (int k = 0; k < layers.size(); k++){
			bValid_per_layer = false;
			for (int i = 0; i < layers[k].symmetry_lines.size(); i++) {
				if (layers[k].symmetry_lines[i].size() > 0){
					bValid_per_layer = true;
					break;
				}
			}
			if (bValid_per_layer){
				validity_layers[k][2] = true;
				layers[k].bUseSymmetryOptValid = true;
				bValid = true;
			}
			else{
				validity_layers[k][2] = false;
				layers[k].bUseSymmetryOptValid = false;
			}
		}
		if (!bValid){
			config.bUseSymmetryLineOpt = false;
			std::cout << "Symmetry invalid" << std::endl;
		}
	}
	// check validity of accuracy
	if (bUseAccuracyOpt){
		for (int k = 0; k < layers.size(); k++){
			validity_layers[k][3] = true;
			layers[k].bUseAccuracyOptValid = true;
		}
	}

	// check validity of point
	if (bUsePointSnapOpt){
		bool bValid_per_layer = false;
		bool bValid = false;
		for (int k = 0; k < layers.size(); k++){
			bValid_per_layer = false;
			for (int i = 0; i < layers[k].contours.size(); i++){
				if (layers[k].parents.size() == 0)
				{
					bValid_per_layer = false;
					break;
				}
				for (int j = 0; j < layers[k].parents.size(); j++){
					if (validPointOpt(layers[k].contours[i], layers[layers[k].parents[j]].contours, pointDisThreshold)){
						std::cout << "layer is " << k << std::endl;
						bValid_per_layer = true;
						break;
					}
				}
				if (bValid_per_layer)
					break;
			}
			if (bValid_per_layer){
				validity_layers[k][4] = true;
				layers[k].bUsePointOptValid = true;
				bValid = true;
			}
			else{
				validity_layers[k][4] = false;
				layers[k].bUsePointOptValid = false;
			}
		}
		if (!bValid){
			config.bUsePointSnapOpt = false;
			std::cout << "PointSnap invalid" << std::endl;
		}
	}

	// check validity of seg
	if (bUseSegSnapOpt){
		bool bValid_per_layer = false;
		bool bValid = false;
		for (int k = 0; k < layers.size(); k++){
			bValid_per_layer = false;
			for (int i = 0; i < layers[k].contours.size(); i++){
				if (layers[k].parents.size() == 0)
				{
					bValid_per_layer = false;
					break;
				}
				for (int j = 0; j < layers[k].parents.size(); j++){
					cv::Rect bbox = cv::boundingRect(layers[k].contours[i]);
					std::cout << "segDisThreshold is " << segDisThreshold << std::endl;
					float dis_threshold = segDisThreshold * 0.01 * sqrt(bbox.width * bbox.width + bbox.height * bbox.height);
					std::cout << "bbox is " << bbox << std::endl;
					std::cout << "dis_threshold is " << dis_threshold << std::endl;
					if (validSegOpt(layers[k].contours[i], layers[layers[k].parents[j]].contours, dis_threshold, segAngleThreshold)){
						bValid_per_layer = true;
						break;
					}
				}
				if (bValid_per_layer)
					break;
			}
			if (bValid_per_layer){
				validity_layers[k][5] = true;
				layers[k].bUseSegOptValid = true;
				bValid = true;
			}
			else{
				validity_layers[k][5] = false;
				layers[k].bUseSegOptValid = false;
			}
		}
		if (!bValid){
			config.bUseSegSnapOpt = false;
			std::cout << "PointSnap invalid" << std::endl;
		}
	}
	// 
	if (!config.bUseRaOpt && !config.bUseParallelOpt && !config.bUseSymmetryLineOpt && !config.bUseAccuracyOpt && !config.bUsePointSnapOpt && !config.bUseSegSnapOpt)
	{
		std::cout << "no need to do optimization!!!" << std::endl;
		return;
	}
	// test validity vector
	/*{
		std::cout << "-------------------------" << std::endl;
		for (int k = 0; k < layers.size(); k++){
			for (int i = 0; i < validity_layers[k].size(); i++){
				std::cout << validity_layers[k][i];
			}
			std::cout << std::endl;
		}
		std::cout << "-------------------------" << std::endl;
	}*/
	try {
		std::cout << "total points is " << total_points << std::endl;
		column_vector starting_point(total_points * 2);
		int start_index = 0;
		for (int k = 0; k < normalized_polygons_init.size(); k++){
			for (int i = 0; i < normalized_polygons_init[k].size(); i++){
				for (int j = 0; j < normalized_polygons_init[k][i].size(); j++){
					starting_point((j + start_index) * 2) = normalized_polygons_init[k][i][j].x;
					starting_point((j + start_index) * 2 + 1) = normalized_polygons_init[k][i][j].y;
				}
				start_index += normalized_polygons_init[k][i].size();
			}
		}
		BFGSSolver solver(normalized_polygons, normalized_polygons_init, normalized_symmetry_lines, layers_height, tree_info, validity_layers, config);
		if (true)
			find_max_using_approximate_derivatives(dlib::bfgs_search_strategy(), dlib::objective_delta_stop_strategy(1e-6), solver, starting_point, 1, 0.0001);
		else
			find_min_using_approximate_derivatives(dlib::lbfgs_search_strategy(total_points), dlib::objective_delta_stop_strategy(1e-6), solver, starting_point, 0, 0.00001);
		start_index = 0;
		std::vector<layer_polygons> new_layers_contours;
		new_layers_contours.resize(layers.size());
		for (int k = 0; k < layers.size(); k++){
			new_layers_contours[k].resize(layers[k].contours.size());
			for (int i = 0; i < layers[k].contours.size(); i++) {
				new_layers_contours[k][i].resize(layers[k].contours[i].size());
				for (int j = 0; j < layers[k].contours[i].size(); j++){
					new_layers_contours[k][i][j].x = starting_point((j + start_index) * 2) * max_unit + min_x;
					new_layers_contours[k][i][j].y = starting_point((j + start_index) * 2 + 1) * max_unit + min_y;
				}
				start_index += layers[k].contours[i].size();
			}
		}
		// reset contours
		for (int k = 0; k < layers.size(); k++){
			layers[k].contours = new_layers_contours[k];
		}
	}
	catch (char* ex) {
		std::cout << ex << std::endl;
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
	catch (...) {
		std::cout << "BFGS optimization failure." << std::endl;
	}

}

bool ShapeFitLayersAll::validRA(const std::vector<cv::Point2f>& polygon, bool bUseRaOpt, int ra_angle_threshold){
	int total_segments = polygon.size();
	if (bUseRaOpt){
		for (int i = 0; i < total_segments; i++){
			int first_start = i;
			int first_end = (i + 1) % total_segments;
			int second_start = (i + 1) % total_segments;
			int second_end = (i + 2) % total_segments;

			// init angles
			cv::Point2f a_init = cv::Point2f(polygon[first_start].x, polygon[first_start].y);
			cv::Point2f b_init = cv::Point2f(polygon[first_end].x, polygon[first_end].y);
			cv::Point2f c_init = cv::Point2f(polygon[second_start].x, polygon[second_start].y);;
			cv::Point2f d_init = cv::Point2f(polygon[second_end].x, polygon[second_end].y);
			float angle_init = util::lineLineAngle(a_init, b_init, c_init, d_init);
			//std::cout << i << " angle is " << angle_init << std::endl;
			// check
			if (abs(angle_init - 45) <= ra_angle_threshold || abs(angle_init - 90) <= ra_angle_threshold || abs(angle_init - 135) <= ra_angle_threshold){
				return true;
			}
		}
		return false;
	}
	return false;
}

bool ShapeFitLayersAll::validParallel(const std::vector<cv::Point2f>& polygon, bool bUseParallelOpt, int parallel_angle_threshold){
	int total_segments = polygon.size();
	if (bUseParallelOpt){
		for (int i = 0; i < total_segments - 1; i++){
			for (int j = i + 1; j < total_segments; j++){
				int first_start = i;
				int first_end = (i + 1) % total_segments;
				int second_start = j;
				int second_end = (j + 1) % total_segments;

				// init angles
				cv::Point2f a_init = cv::Point2f(polygon[first_start].x, polygon[first_start].y);
				cv::Point2f b_init = cv::Point2f(polygon[first_end].x, polygon[first_end].y);
				cv::Point2f c_init = cv::Point2f(polygon[second_start].x, polygon[second_start].y);;
				cv::Point2f d_init = cv::Point2f(polygon[second_end].x, polygon[second_end].y);
				float angle_init = util::lineLineAngle(a_init, b_init, c_init, d_init);
				//std::cout << i <<" to "<<j <<" angle is " << angle_init << std::endl;
				// check
				if (abs(angle_init) <= parallel_angle_threshold || abs(angle_init - 180) <= parallel_angle_threshold)
					return true;
			}
		}
		return false;
	}
	return false;
}

bool ShapeFitLayersAll::validPointOpt(const std::vector<cv::Point2f>& src_polygon, const std::vector<std::vector<cv::Point2f>>& des_layer_polygons, float threshold){
	//cv::Rect bbox = cv::boundingRect(src_polygon);
	//float dis_threshold = threshold * 0.01 * sqrt(bbox.width * bbox.width + bbox.height * bbox.height);
	//std::cout << "bbox is " << bbox << std::endl;
	//std::cout << "dis_threshold is " << dis_threshold << std::endl;
	for (int k = 0; k < src_polygon.size(); k++){// current source polygon
		for (int i = 0; i < des_layer_polygons.size(); i++){// all polygons in the destination layer
			cv::Rect bbox = cv::boundingRect(des_layer_polygons[i]);
			float dis_threshold = threshold * 0.015 * sqrt(bbox.width * bbox.width + bbox.height * bbox.height);
			std::cout << "bbox is " << bbox << std::endl;
			std::cout << "dis_threshold is " << dis_threshold << std::endl;
			for (int j = 0; j < des_layer_polygons[i].size(); j++){// one polygon in the destination layer
				cv::Point2f src_p = src_polygon[k];
				cv::Point2f des_p = des_layer_polygons[i][j];
				float dis = cv::norm(src_p - des_p);
				if (dis <= dis_threshold)
					return true;
				//std::cout << "dis_init from " << k << " to " << j << " is " << dis << std::endl;
			}
		}
	}
	return false;
}

bool ShapeFitLayersAll::validSegOpt(const std::vector<cv::Point2f>& src_polygon, const std::vector<std::vector<cv::Point2f>>& des_layer_polygons, float threshold, float angle_threshold){
	int total_seg_src = src_polygon.size();
	for (int k = 0; k < total_seg_src; k++){// current source polygon
		for (int i = 0; i < des_layer_polygons.size(); i++){// all polygons in the destination layer
			int total_seg_des = des_layer_polygons[i].size();

			cv::Rect bbox = cv::boundingRect(des_layer_polygons[i]);
			float dis_threshold = threshold * 0.015 * sqrt(bbox.width * bbox.width + bbox.height * bbox.height);
			std::cout << "bbox is " << bbox << std::endl;
			std::cout << "dis_threshold is " << dis_threshold << std::endl;

			for (int j = 0; j < total_seg_des; j++){// one polygon in the destination layer
				// angle score
				cv::Point2f src_start = src_polygon[k];
				cv::Point2f src_end = src_polygon[(k + 1) % total_seg_src];
				cv::Point2f des_start = des_layer_polygons[i][j];
				cv::Point2f des_end = des_layer_polygons[i][(j + 1) % total_seg_des];
				float angle = util::lineLineAngle(src_start, src_end, des_start, des_end);
				float dis = util::distance(src_start, src_end, des_start, des_end);
				angle = std::min(abs(angle), abs(180 - angle));
				if (angle <= angle_threshold && dis <= dis_threshold)
					return true;
			}
		}
	}
	return false;
}