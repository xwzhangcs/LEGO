#include "../LEGO/OpenCVSimplification.h"

int main() {
	std::string filename = "data/slice_001.png";
	std::vector<cv::Mat> voxel_data;

	/*
	// get directory
	QDir dir = QFileInfo(filename).absoluteDir();

	// scan all the files in the directory to get a voxel data
	QStringList files = dir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::DirsFirst);
	voxel_data.resize(files.size());
	for (int i = 0; i < files.size(); i++) {
		voxel_data[i] = cv::imread((dir.absolutePath() + "/" + files[i]).toUtf8().constData(), cv::IMREAD_GRAYSCALE);
	}
	*/

	std::vector<Building> buildings;

	lego::OpenCVSimplification sim(voxel_data, 1, 0.7, 1, 0.5);
	sim.simplify(buildings);

	return 0;
}