#include <iostream>
#include "OpenCVSimplification.h"
#include <QDir>
#include <QString>
#include "PlyWriter.h"

int main(int argc, const char* argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <slice filename>" << std::endl;
		return -1;
	}

	QString filename(argv[1]);
	std::vector<cv::Mat> voxel_data;

	// get directory
	QFileInfo finfo(filename);
	if (!finfo.exists()) {
		std::cerr << "File " << argv[0] << " was not found." << std::endl;
		return -1;
	}
	QDir dir = finfo.absoluteDir();

	// scan all the files in the directory to get a voxel data
	QStringList files = dir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::DirsFirst);
	voxel_data.resize(files.size());
	for (int i = 0; i < files.size(); i++) {
		voxel_data[i] = cv::imread((dir.absolutePath() + "/" + files[i]).toUtf8().constData(), cv::IMREAD_GRAYSCALE);
	}

	std::vector<simp::Building> buildings;

	simp::OpenCVSimplification sim(voxel_data, 1, 0.7, 1, 0.5);
	sim.simplify(buildings);

	util::ply::PlyWriter::write("buildings.ply", buildings);

	std::cout << buildings.size() << " buildings are generated." << std::endl;

	return 0;
}