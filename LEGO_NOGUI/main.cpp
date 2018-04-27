#include <iostream>
#include <QDir>
#include <QString>
#include "util/DisjointVoxelData.h"
#include "simp/BuildingSimplification.h"
#include "util/OBJWriter.h"
#include "util/TopFaceWriter.h"
#include "util/PlyWriter.h"

int main(int argc, const char* argv[]) {
	if (argc < 9) {
		std::cerr << "Usage: " << argv[0] << " <slice image filename> <weight [0-1]> <algorithm option: 1 - All, 2 - DP> <offset_x> <offset_y> <offset_z> <scale> <output obj filename> <output topface filename>" << std::endl;
		return -1;
	}

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
	float alpha = std::stof(argv[2]);
	alpha = std::min(std::max(0.0f, alpha), 1.0f);

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
	float threshold = 0.01;
	if (alpha < 0.1) threshold = 0.1;
	else if (alpha < 0.2) threshold = 0.3;
	else if (alpha < 0.3) threshold = 0.4;
	else if (alpha < 0.4) threshold = 0.5;
	else if (alpha < 0.5) threshold = 0.6;
	else if (alpha < 0.7) threshold = 0.6;
	else if (alpha < 0.9) threshold = 0.7;
	else threshold = 0.99;

	// determine the parameter for the DP method
	float epsilon;
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
	float curve_threshold;
	if (alpha < 0.2) curve_threshold = 2.0f;
	else curve_threshold = 2.0f;
	float angle_threshold = 10.0f / 180.0f * CV_PI;
	if (alpha < 0.2) angle_threshold = 20.0f / 180.0f * CV_PI;

	int min_num_slices_per_layer = 2.5 / scale;
	float min_hole_ratio = 0.02;

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

	return 0;
}