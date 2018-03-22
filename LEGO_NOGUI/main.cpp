#include <iostream>
#include <QDir>
#include <QString>
#include "util/DisjointVoxelData.h"
#include "simp/BuildingSimplification.h"
#include "util/PlyWriter.h"

int main(int argc, const char* argv[]) {
	if (argc < 9) {
		std::cerr << "Usage: " << argv[0] << " <slice image filename> <weight [0-1]> <algorithm option: 1 - All, 2 - DP> <offset_x> <offset_y> <offset_z> <scale> <output filename>" << std::endl;
		return -1;
	}

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
	float threshold;
	if (alpha < 0.2) threshold = 0.0;
	else if (alpha < 0.4) threshold = 0.1;
	else if (alpha < 0.6) threshold = 0.4;
	else if (alpha < 0.7) threshold = 0.5;
	else if (alpha < 0.8) threshold = 0.6;
	else if (alpha < 1.0) threshold = 0.9;
	else threshold = 1.0;

	std::vector<std::shared_ptr<util::BuildingLayer>> raw_buildings = util::DisjointVoxelData::disjoint(voxel_data);

	std::vector<std::shared_ptr<util::BuildingLayer>> buildings;
	if (std::stoi(argv[3]) == 1) {
		buildings = simp::BuildingSimplification::simplifyBuildings(raw_buildings, simp::BuildingSimplification::ALG_ALL, alpha, 0.5, 2, 4, 1, 0);
	}
	else if (std::stoi(argv[3]) == 2) {
		buildings = simp::BuildingSimplification::simplifyBuildings(raw_buildings, simp::BuildingSimplification::ALG_DP, alpha, 0.5, 2, 4, 1, 0);
	}
	util::ply::PlyWriter::write(argv[8], std::stod(argv[4]), std::stod(argv[5]), std::stod(argv[6]), std::stod(argv[7]), buildings);

	std::cout << buildings.size() << " buildings are generated." << std::endl;

	return 0;
}