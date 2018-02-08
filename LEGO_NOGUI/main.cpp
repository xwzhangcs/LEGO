#include <iostream>
#include "OpenCVSimplification.h"
#include <QDir>
#include <QString>
#include "DisjointVoxelData.h"
#include "BuildingSimplification.h"
#include "PlyWriter.h"

int main(int argc, const char* argv[]) {
	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " <slice image filename> <output filename>" << std::endl;
		return -1;
	}

	QString input_filename(argv[1]);
	std::vector<cv::Mat_<uchar>> voxel_data;

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

	util::DisjointVoxelData dvd;
	dvd.disjoint(voxel_data, 0.5);

	std::vector<std::shared_ptr<simp::Building>> buildings = simp::BuildingSimplification::simplifyBuildings(dvd, simp::BuildingSimplification::ALG_OPENCV, 0.8, 1.0, 1.0, 1, 4, 2);
	util::ply::PlyWriter::write(argv[2], buildings);

	std::cout << buildings.size() << " buildings are generated." << std::endl;

	return 0;
}