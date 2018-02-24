#include <iostream>
#include <QDir>
#include <QString>
#include "util/DisjointVoxelData.h"
#include "simp/BuildingSimplification.h"
#include "util/PlyWriter.h"

int main(int argc, const char* argv[]) {
	if (argc < 4) {
		std::cerr << "Usage: " << argv[0] << " <slice image filename> <weight [0-1]> <output filename>" << std::endl;
		return -1;
	}

	QString input_filename(argv[1]);
	std::vector<cv::Mat_<uchar>> voxel_data;

	float alpha = std::stof(argv[2]);

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

	// find the ground level
	int ground_level = 0;
	double max_area = 0;
	for (int i = 0; i < voxel_data.size(); i++) {
		max_area = std::max(max_area, util::calculateArea(voxel_data[i]));
	}
	for (ground_level = 0; ground_level < voxel_data.size(); ground_level++) {
		// If the slice has voxel more than 80% of the maximum, the slice is considered at the ground level.
		if (util::calculateArea(voxel_data[ground_level]) > max_area * 0.8) break;
	}

	util::DisjointVoxelData dvd;
	dvd.disjoint(voxel_data, 0.5);

	std::vector<std::shared_ptr<util::BuildingLayer>> buildings = simp::BuildingSimplification::simplifyBuildings(dvd, ground_level, simp::BuildingSimplification::ALG_ALL, alpha, 0.1, 2, 4, 1);
	util::ply::PlyWriter::write(argv[3], buildings);

	std::cout << buildings.size() << " buildings are generated." << std::endl;

	return 0;
}