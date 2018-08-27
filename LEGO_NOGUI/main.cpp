#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <QDir>
#include <QString>
#include <QTextStream>
#include "util/DisjointVoxelData.h"
#include "simp/BuildingSimplification.h"
#include "util/OBJWriter.h"
#include "util/TopFaceWriter.h"
#include "util/PlyWriter.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "regularizer/Config.h"

#include "voxel_model.h"


double readDoubleValue(const rapidjson::Value& node, const char* key) {
	if (node.HasMember(key) && node[key].IsDouble()) {
		return node[key].GetDouble();
	}
	else {
	  throw "Could not read double from node";
	}
}

double readNumber(const rapidjson::Value& node, const char* key, double default_value) {
	if (node.HasMember(key) && node[key].IsDouble()) {
		return node[key].GetDouble();
	}
	else if (node.HasMember(key) && node[key].IsInt()) {
		return node[key].GetInt();
	}
	else {
		return default_value;
	}
}

bool readBoolValue(const rapidjson::Value& node, const char* key, bool default_value) {
	if (node.HasMember(key) && node[key].IsBool()) {
		return node[key].GetBool();
	}
	else if (node.HasMember(key) && node[key].IsString()) {
		return QString(node[key].GetString()).toLower() == "true";
	}
	else {
		return default_value;
	}
}

QString readStringValue(const rapidjson::Value& node, const char* key) {
	if (node.HasMember(key) && node[key].IsString()) {
		return QString(node[key].GetString());
	}
	else {
		throw "Could not read string from node";
	}
}

std::vector<double> readAlgorithmParams(rapidjson::Value& node, const QString& algorithm_name ) {
	rapidjson::Value::ConstMemberIterator it = node.FindMember(algorithm_name.toUtf8().constData());
	if (it != node.MemberEnd()) {
		rapidjson::Value& alg = node.FindMember(algorithm_name.toUtf8().constData())->value;
		rapidjson::Value& use = alg["use"];
		if (use.IsBool() && use.GetBool()) {
			if (algorithm_name == "douglas_peucker") {
				double epsilon = readNumber(alg, "epsilon", 16);
				return{ epsilon };
			}
			else if (algorithm_name == "right_angle") {
				double epsilon = readNumber(alg, "epsilon", 20);
				bool optimization = readBoolValue(alg, "optimization", true);
				return{ epsilon, optimization ? 1.0 : 0.0 };
			}
			else if (algorithm_name == "curve") {
				double epsilon = readNumber(alg, "epsilon", 16);
				double curve_threshold = readNumber(alg, "curve_threshold", 2);
				return{ epsilon, curve_threshold };
			}
			else if (algorithm_name == "curvepp") {
				double epsilon = readNumber(alg, "epsilon", 16);
				double curve_threshold = readNumber(alg, "curve_threshold", 2);
				double angle_threshold = readNumber(alg, "angle_threshold", 10) / 180.0 * CV_PI;
				return{ epsilon, curve_threshold, angle_threshold };
			}
			else if (algorithm_name == "efficient_ransac") {
				//curve
				rapidjson::Value& algs_curve = alg["Curve"];
				double curve_num_iterations = readNumber(algs_curve, "iterations", 200000);
				double curve_min_points = readNumber(algs_curve, "min_points", 200);
				double curve_max_error_ratio_to_radius = readNumber(algs_curve, "max_error_ratio_to_radius", 0.02);
				double curve_cluster_epsilon = readNumber(algs_curve, "cluster_epsilon", 30);
				double curve_min_angle = readNumber(algs_curve, "min_angle", 90) / 180.0 * CV_PI;
				double curve_min_radius = readNumber(algs_curve, "min_radius", 80); 
				double curve_max_radius = readNumber(algs_curve, "max_radius", 400);
				//line
				rapidjson::Value& algs_line = alg["Line"];
				double line_num_iterations = readNumber(algs_line, "iterations", 20000);
				double line_min_points = readNumber(algs_line, "min_points", 50);
				double line_max_error = readNumber(algs_line, "max_error", 5);
				double line_cluster_epsilon = readNumber(algs_line, "cluster_epsilon", 20);
				double line_min_length = readNumber(algs_line, "min_length", 50); 
				double line_angle_threshold = readNumber(algs_line, "angle_threshold", 15) / 180.0 * CV_PI;
				//contour
				rapidjson::Value& algs_contour = alg["Contour"];
				double contour_max_error = readNumber(algs_contour, "max_error", 20);
				double contour_angle_threshold = readNumber(algs_contour, "angle_threshold", 10) / 180.0 * CV_PI;
				
				return{ curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold, contour_max_error, contour_angle_threshold };
			}
		}
	}

	throw "Invalid or unselected algorithm.";
}

std::vector<regularizer::Config> readRegularizerParams(rapidjson::Value& node) {
	std::vector<regularizer::Config> regularizer_configs;
	int num_runs = node["num_runs"].GetInt();
	regularizer_configs.resize(num_runs);
	for (int i = 0; i < num_runs; i++){
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
		float symmetryIouThreshold = 0.0f;
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

		// get parameters for each run
		QString run_name = "Opt" + QString::number(i + 1);
		rapidjson::Value& algs_run = node[run_name.toUtf8().constData()];

		bUseIntra = readBoolValue(algs_run, "UseIntra", false);
		bUseInter = readBoolValue(algs_run, "UseInter", false);
		intraWeight = readNumber(algs_run, "Intra_Weight", 0.5);
		interWeight = readNumber(algs_run, "Inter_Weight", 0.5);

		if (bUseIntra){
			rapidjson::Value& algs = algs_run["IntraOpt"];
			//ra
			rapidjson::Value& algs_ra = algs["RA"];
			bUseRaOpt = readBoolValue(algs_ra, "UseOpt", false);
			angle_threshold_RA = readNumber(algs_ra, "AngleThreshold", 10.0); 
			raWeight = readNumber(algs_ra, "Weight", 1.0);
			//symmetry
			rapidjson::Value& algs_symmetry = algs["Symmetry"];
			bUseSymmetryLineOpt = readBoolValue(algs_symmetry, "UseOpt", false);
			symmetryIouThreshold = readNumber(algs_symmetry, "IouThreshold", 90.0);
			symmetryWeight = readNumber(algs_symmetry, "Weight", 1.0);
			// parallel
			rapidjson::Value& algs_parallel = algs["Parallel"];
			bUseParallelOpt = readBoolValue(algs_parallel, "UseOpt", false);
			angle_threshold_parallel = readNumber(algs_parallel, "AngleThreshold", 10.0);
			parallelWeight = readNumber(algs_parallel, "Weight", 1.0);
			// accuracy
			rapidjson::Value& algs_accuracy = algs["Accuracy"];
			bUseAccuracyOpt = readBoolValue(algs_accuracy, "UseOpt", false);
			accuracyWeight = readNumber(algs_accuracy, "Weight", 1.0);
		}
		if (bUseInter){
			rapidjson::Value& algs = algs_run["InterOpt"];
			// point snap
			rapidjson::Value& algs_point = algs["PointSnap"];
			bUsePointSnapOpt = readBoolValue(algs_point, "UseOpt", false);
			pointDisThreshold = readNumber(algs_point, "DisThreshold", 10); 
			pointWeight = readNumber(algs_point, "Weight", 1.0);
			// seg snap
			rapidjson::Value& algs_seg = algs["SegSnap"];
			bUseSegSnapOpt = readBoolValue(algs_seg, "UseOpt", false);
			segDisThreshold = readNumber(algs_seg, "DisThreshold", 10);
			segAngleThreshold = readNumber(algs_seg, "AngleThreshold", 5);
			segWeight = readNumber(algs_seg, "Weight", 1.0);
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
				// do nothing
			}
			else{
				throw "Please invalid Intra and Inter weight assignment";
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
				// do nothing
			}
			else{
				throw "Please invalid Intra weight assignment";
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
				// do nothing
			}
			else{
				throw "Please invalid Inter weight assignment";
			}
		}
		regularizer::Config config(bUseIntra, intraWeight, bUseInter, interWeight, bUseRaOpt, angle_threshold_RA, raWeight, bUseParallelOpt, angle_threshold_parallel, parallelWeight, bUseSymmetryLineOpt, symmetryIouThreshold, symmetryWeight, bUseAccuracyOpt, accuracyWeight, bUsePointSnapOpt, pointDisThreshold, pointWeight, bUseSegSnapOpt, segDisThreshold, segAngleThreshold, segWeight);
		regularizer_configs[i] = config;
	}
	return regularizer_configs;
}

int main(int argc, const char* argv[]) {
	if (argc == 2) {
		////////////////////////////////////////////////////////////////////////////////////
		// the new interface
		QFile file(argv[1]);
		if (!file.open(QIODevice::ReadOnly)) {
			std::cerr << "File was not readable: " << argv[1] << std::endl;
			return -1;
		}
		QTextStream in(&file);
		rapidjson::Document doc;
		doc.Parse(in.readAll().toUtf8().constData());
		
		std::vector<cv::Mat_<uchar>> voxel_data;
		
		// read input filename
		bool do_voxel_model = readBoolValue(doc, "do_voxel_model", false);
		
		// read input filename
		QString input_slice_filename;
		try {
			input_slice_filename = readStringValue(doc, "input_slice_filename");
		}
		catch (...) {
			std::cerr << "Invalid data for input_slice_filename in the json file." << std::endl;
			return -1;
		}
		
		// read output obj output_mesh
		QString output_mesh;
		try {
			output_mesh = readStringValue(doc, "output_mesh");
		}
		catch (...) {
			std::cerr << "Invalid data for output_mesh in the json file." << std::endl;
			return -1;
		}
		
		// read output topface filename
		QString output_top_face;
		try {
			output_top_face = readStringValue(doc, "output_top_face");
		}
		catch (...) {
			std::cerr << "Invalid data for output_top_face in the json file." << std::endl;
			return -1;
		}

		// get directory
		QFileInfo finfo(input_slice_filename);
		if (!finfo.exists()) {
			std::cerr << "Input file was not found: " << input_slice_filename.toUtf8().constData() << std::endl;
			return -1;
		}
		QDir dir = finfo.absoluteDir();
		
		// scan all the files in the directory to get a voxel data
		QStringList files = dir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::DirsFirst);
		voxel_data.resize(files.size());
		for (int i = 0; i < files.size(); i++) {
			voxel_data[i] = cv::imread((dir.absolutePath() + "/" + files[i]).toUtf8().constData(), cv::IMREAD_GRAYSCALE);
		}
		
		// the following 4 parameters are necessary, we should throw an error if they are not provided, so removing defautl values.
		
		// read offset_x
		double offset_x = readDoubleValue(doc, "offset_x");

		// read offset_y
		double offset_y = readDoubleValue(doc, "offset_y");

		// read offset_z
		double offset_z = readDoubleValue(doc, "offset_z");

		// read scale
		double scale = readDoubleValue(doc, "scale");

		// do voxel model (for debug)
		if (do_voxel_model) {
		  std::cout<< "Doing voxel model" << std::endl;
		  voxel_model::voxel_model(voxel_data, output_mesh.toStdString(), offset_x, offset_y, offset_z, scale);
		  return 0;
		}
		
		// read contour simplificaton weight
		double contour_simplification_weight = readNumber(doc, "contour_simplification_weight", 0.5);
		
		// read layering threshold
		double layering_threshold = readNumber(doc, "layering_threshold", 0.7);

		// read snapping threshold
		double contour_snapping_threshold = readNumber(doc, "contour_snapping_threshold", 2.5) / scale;
				
		// read orientation
		double orientation = readNumber(doc, "bulk_orientation", 0.0) / 180.0 * CV_PI;

		// read minimum contour area
		double min_contour_area = readNumber(doc, "minimum_contour_area", 2.0) / scale / scale;

		// read maximum obb ratio
		double max_obb_ratio = readNumber(doc, "maximum_obb_ratio", 10.0);

		// read the flag whether a triangle contour is allowed
		bool allow_triangle_contour = readBoolValue(doc, "allow_triangle_contour", false);

		// read the flag whether overhang is allowed
		bool allow_overhang = readBoolValue(doc, "allow_overhang", false);

		// read minimum hole ratio
		double min_hole_ratio = readNumber(doc, "min_hole_ratio", 0.02);

		// read minimum height of layer
		double min_layer_height = readNumber(doc, "minimum_layer_height", 2.5) / scale;

		// read algorithms
		std::map<int, std::vector<double>> algorithms;
		rapidjson::Value& algs = doc["contour_simplification_algorithms"];			
		
		try {
			algorithms[simp::BuildingSimplification::ALG_DP] = readAlgorithmParams(algs, "douglas_peucker");
		}
		catch (...) {
		}
		try {
			algorithms[simp::BuildingSimplification::ALG_RIGHTANGLE] = readAlgorithmParams(algs, "right_angle");
		}
		catch (...) {
		}
		try {
			algorithms[simp::BuildingSimplification::ALG_CURVE] = readAlgorithmParams(algs, "curve");
		}
		catch (...) {
		}
		try {
			algorithms[simp::BuildingSimplification::ALG_CURVE_RIGHTANGLE] = readAlgorithmParams(algs, "curvepp");
		}
		catch (...) {
		}
		try {
			algorithms[simp::BuildingSimplification::ALG_EFFICIENT_RANSAC] = readAlgorithmParams(algs, "efficient_ransac");
		}
		catch (...) {
		}

		// read regularizer
		std::vector<regularizer::Config> regularizer_configs = readRegularizerParams(doc["regularizer"]);

		std::vector<util::VoxelBuilding> voxel_buildings = util::DisjointVoxelData::disjoint(voxel_data);

		std::vector<std::shared_ptr<util::BuildingLayer>> buildings;
		buildings = simp::BuildingSimplification::simplifyBuildings(voxel_buildings, algorithms, false, min_layer_height, contour_simplification_weight, layering_threshold, contour_snapping_threshold, orientation, min_contour_area, max_obb_ratio, allow_triangle_contour, allow_overhang, min_hole_ratio, regularizer_configs);

		util::obj::OBJWriter::write(output_mesh.toUtf8().constData(), voxel_data[0].cols, voxel_data[0].rows, offset_x, offset_y, offset_z, scale, buildings);
		util::topface::TopFaceWriter::write(output_top_face.toUtf8().constData(), voxel_data[0].cols, voxel_data[0].rows, offset_x, offset_y, offset_z, scale, buildings);

		std::cout << buildings.size() << " buildings are generated." << std::endl;
	}
	else {
		if (argc < 9) {
			std::cerr << "Usage: \n" << argv[0] << " <slice image filename> <weight [0-1]> <algorithm option: 1 - All, 2 - DP> <offset_x> <offset_y> <offset_z> <scale> <output obj filename> <output topface filename>" << std::endl;
			std::cerr << argv[0] << " <config json filename> " << std::endl;
			return -1;
		}

		////////////////////////////////////////////////////////////////////////////////////
		// the old interface

		std::string topface_file;
		bool record_stats = false;
		if (argc >= 10) {
			topface_file = std::string(argv[9]);
		}
		if (argc == 11 && std::string(argv[10]) == "--log") {
			record_stats = true;
		}

		double offset_x = std::stod(argv[4]);
		double offset_y = std::stod(argv[5]);
		double offset_z = std::stod(argv[6]);
		double scale = std::stod(argv[7]);

		QString input_filename(argv[1]);
		std::vector<cv::Mat_<uchar>> voxel_data;

		// The weight ratio of the accuracy term to the simplicity term for the cost function.
		double alpha = std::stod(argv[2]);
		alpha = std::min(std::max(0.0, alpha), 1.0);

		// get directory
		QFileInfo finfo(input_filename);
		if (!finfo.exists()) {
			std::cerr << "Input file was not found: " << argv[1] << std::endl;
			return -1;
		}
		QDir dir = finfo.absoluteDir();

		// scan all the files in the directory to get a voxel data
		QStringList files = dir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::DirsFirst);
		voxel_data.resize(files.size());
		for (int i = 0; i < files.size(); i++) {
			voxel_data[i] = cv::imread((dir.absolutePath() + "/" + files[i]).toUtf8().constData(), cv::IMREAD_GRAYSCALE);
		}	
		
		// determine the layering threshold based on the weight ratio
		double threshold = 0.01;
		if (alpha < 0.1) threshold = 0.1;
		else if (alpha < 0.2) threshold = 0.3;
		else if (alpha < 0.3) threshold = 0.4;
		else if (alpha < 0.4) threshold = 0.5;
		else if (alpha < 0.5) threshold = 0.6;
		else if (alpha < 0.7) threshold = 0.6;
		else if (alpha < 0.9) threshold = 0.7;
		else threshold = 0.99;

		// determine the parameter for the DP method
		double epsilon;
		if (alpha <= 0.06) epsilon = 24;
		else if (alpha < 0.1) epsilon = 18;
		else if (alpha < 0.2) epsilon = 12;
		else if (alpha < 0.4) epsilon = 10;
		else if (alpha < 0.6) epsilon = 8;
		else if (alpha < 0.8) epsilon = 4;
		else if (alpha < 0.9) epsilon = 4;
		else epsilon = 2;

		// determine the parameter for the right angle method
		int resolution;
		if (alpha <= 0.06) resolution = 24;
		else if (alpha < 0.1) resolution = 18;
		else if (alpha < 0.2) resolution = 12;
		else if (alpha < 0.4) resolution = 10;
		else if (alpha < 0.6) resolution = 8;
		else if (alpha < 0.8) resolution = 4;
		else if (alpha < 0.9) resolution = 4;
		else resolution = 2;
		
		// determine the parameter for the curve method
		double curve_threshold;
		if (alpha < 0.2) curve_threshold = 2.0;
		else curve_threshold = 2.0f;
		double angle_threshold = 10.0 / 180.0 * CV_PI;
		if (alpha < 0.2) angle_threshold = 20.0 / 180.0 * CV_PI;

		int min_num_slices_per_layer = 2.5 / scale;
		double min_hole_ratio = 0.02;

		time_t start = clock();
		std::vector<util::VoxelBuilding> voxel_buildings = util::DisjointVoxelData::disjoint(voxel_data);
		time_t end = clock();
		std::cout << "Time elapsed: " << (double)(end - start) / CLOCKS_PER_SEC << " sec." << std::endl;
		
		std::vector<std::shared_ptr<util::BuildingLayer>> buildings;
		if (std::stoi(argv[3]) == 1) {
			buildings = simp::BuildingSimplification::simplifyBuildings(voxel_buildings, simp::BuildingSimplification::ALG_ALL, record_stats, min_num_slices_per_layer, alpha, threshold, epsilon, resolution, curve_threshold, angle_threshold, min_hole_ratio);
		}
		else if (std::stoi(argv[3]) == 2) {
			buildings = simp::BuildingSimplification::simplifyBuildings(voxel_buildings, simp::BuildingSimplification::ALG_DP, record_stats, min_num_slices_per_layer, alpha, threshold, epsilon, resolution, curve_threshold, angle_threshold, min_hole_ratio);
		}
		else if (std::stoi(argv[3]) == 3) {
			buildings = simp::BuildingSimplification::simplifyBuildings(voxel_buildings, simp::BuildingSimplification::ALG_RIGHTANGLE, record_stats, min_num_slices_per_layer, alpha, threshold, epsilon, resolution, curve_threshold, angle_threshold, min_hole_ratio);
		}
		else if (std::stoi(argv[3]) == 4) {
			buildings = simp::BuildingSimplification::simplifyBuildings(voxel_buildings, simp::BuildingSimplification::ALG_CURVE, record_stats, min_num_slices_per_layer, alpha, threshold, epsilon, resolution, curve_threshold, angle_threshold, min_hole_ratio);
		}
		else if (std::stoi(argv[3]) == 5) {
			buildings = simp::BuildingSimplification::simplifyBuildings(voxel_buildings, simp::BuildingSimplification::ALG_CURVE_RIGHTANGLE, record_stats, min_num_slices_per_layer, alpha, threshold, epsilon, resolution, curve_threshold, angle_threshold, min_hole_ratio);
		}

		util::obj::OBJWriter::write(std::string(argv[8]), voxel_data[0].cols, voxel_data[0].rows, offset_x, offset_y, offset_z, scale, buildings);
		if (topface_file.size() > 0) {
			util::topface::TopFaceWriter::write(topface_file, voxel_data[0].cols, voxel_data[0].rows, offset_x, offset_y, offset_z, scale, buildings);
		}

		std::cout << buildings.size() << " buildings are generated." << std::endl;
	}

	return 0;
}
