#include "MainWindow.h"
#include <iostream>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>
#include "AllOptionDialog.h"
#include "DPOptionDialog.h"
#include "RightAngleOptionDialog.h"
#include "CurveOptionDialog.h"
#include "CurveRightAngleOptionDialog.h"
#include "EfficientRANSACOptionDialog.h"
#include "GenerateFacadeOptionDialog.h"
#include "OffsetScaleDialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	// group for simplification modes
	QActionGroup* groupSimplify = new QActionGroup(this);
	groupSimplify->addAction(ui.actionInputVoxel);
	groupSimplify->addAction(ui.actionSimplifyByAll);
	groupSimplify->addAction(ui.actionSimplifyByDP);
	groupSimplify->addAction(ui.actionSimplifyByRightAngle);
	groupSimplify->addAction(ui.actionSimplifyByCurve);
	groupSimplify->addAction(ui.actionSimplifyByCurveRightAngle);

	// group for rendering modes
	QActionGroup* groupColoring = new QActionGroup(this);
	groupColoring->addAction(ui.actionColor);
	groupColoring->addAction(ui.actionTexture);

	// group for rendering modes
	QActionGroup* groupRendering = new QActionGroup(this);
	groupRendering->addAction(ui.actionRenderingBasic);
	groupRendering->addAction(ui.actionRenderingSSAO);
	groupRendering->addAction(ui.actionRenderingHatching);

	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionSaveOBJ, SIGNAL(triggered()), this, SLOT(onSaveOBJ()));
	connect(ui.actionSaveTopFaces, SIGNAL(triggered()), this, SLOT(onSaveTopFaces()));
	connect(ui.actionSavePLY, SIGNAL(triggered()), this, SLOT(onSavePLY()));
	connect(ui.actionSaveImage, SIGNAL(triggered()), this, SLOT(onSaveImage()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionInputVoxel, SIGNAL(triggered()), this, SLOT(onInputVoxel()));
	connect(ui.actionSimplifyByAll, SIGNAL(triggered()), this, SLOT(onSimplifyByAll()));
	connect(ui.actionSimplifyByDP, SIGNAL(triggered()), this, SLOT(onSimplifyByDP()));
	connect(ui.actionSimplifyByRightAngle, SIGNAL(triggered()), this, SLOT(onSimplifyByRightAngle()));
	connect(ui.actionSimplifyByCurve, SIGNAL(triggered()), this, SLOT(onSimplifyByCurve()));
	connect(ui.actionSimplifyByCurveRightAngle, SIGNAL(triggered()), this, SLOT(onSimplifyByCurveRightAngle()));
	connect(ui.actionSimplifyByEfficientRANSAC, SIGNAL(triggered()), this, SLOT(onSimplifyByEfficientRANSAC()));
	connect(ui.actionOffsetScale, SIGNAL(triggered()), this, SLOT(onOffsetScale()));
	connect(ui.actionColor, SIGNAL(triggered()), this, SLOT(onColoringModeChanged()));
	connect(ui.actionTexture, SIGNAL(triggered()), this, SLOT(onColoringModeChanged()));
	connect(ui.actionRenderingBasic, SIGNAL(triggered()), this, SLOT(onRenderingModeChanged()));
	connect(ui.actionRenderingSSAO, SIGNAL(triggered()), this, SLOT(onRenderingModeChanged()));
	connect(ui.actionRenderingHatching, SIGNAL(triggered()), this, SLOT(onRenderingModeChanged()));
	connect(ui.actionGenerateFacadeImages, SIGNAL(triggered()), this, SLOT(onGenerateFacadeImages()));
	connect(ui.actionGenerateRectifiedImage, SIGNAL(triggered()), this, SLOT(onGenerateRectifiedImage()));

	// create tool bar for file menu
	ui.mainToolBar->addAction(ui.actionOpen);
	ui.mainToolBar->addAction(ui.actionSaveOBJ);

	// setup the GL widget
	glWidget = new GLWidget3D(this);
	setCentralWidget(glWidget);
}

MainWindow::~MainWindow() {
}

void MainWindow::onOpen() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load voxel data..."), "", tr("Image files (*.png *.jpg *.bmp)"));
	if (filename.isEmpty()) return;

	setWindowTitle("LEGO - " + filename);
	glWidget->loadVoxelData(filename);
	glWidget->update();
}

void MainWindow::onSaveOBJ() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save OBJ file..."), "", tr("OBJ files (*.obj)"));
	if (!filename.isEmpty()) {
		glWidget->saveOBJ(filename);
	}
}

void MainWindow::onSaveTopFaces() {
	if (glWidget->show_mode == GLWidget3D::SHOW_INPUT) {
		QMessageBox msg;
		msg.setText("Simplify the buildings.");
		msg.exec();
		return;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Save text file..."), "", tr("text files (*.txt)"));
	if (!filename.isEmpty()) {
		glWidget->saveTopFace(filename);
	}
}

void MainWindow::onSavePLY() {
	if (glWidget->show_mode == GLWidget3D::SHOW_INPUT) {
		QMessageBox msg;
		msg.setText("Simplify the buildings.");
		msg.exec();
		return;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Save PLY file..."), "", tr("PLY files (*.ply)"));
	if (!filename.isEmpty()) {
		glWidget->savePLY(filename);
	}
}

void MainWindow::onSaveImage() {
	if (!QDir("screenshot").exists()) {
		QDir().mkdir("screenshot");
	}
	QDateTime dateTime = QDateTime().currentDateTime();
	QString str = QString("screenshot/") + dateTime.toString("yyyyMMddhhmmss") + QString(".png");

	glWidget->saveImage(str);
}

void MainWindow::onInputVoxel() {
	glWidget->showInputVoxel();
	glWidget->update();
}

void MainWindow::onSimplifyByAll() {
	AllOptionDialog dlg;
	if (dlg.exec()) {
		glWidget->simplifyByAll(dlg.getAlpha());
		glWidget->update();
	}
}

void MainWindow::onSimplifyByDP() {
	DPOptionDialog dlg;
	if (dlg.exec()) {
		glWidget->simplifyByDP(dlg.getEpsilon(), dlg.getLayeringThreshold(), dlg.getSnappingThreshold(), dlg.getOrientation() / 180.0 * CV_PI, dlg.getMinContourArea(), dlg.getMaxOBBRatio(), dlg.isAllowTriangleContour(), dlg.isAllowOverhang());
		glWidget->update();
	}
}

void MainWindow::onSimplifyByRightAngle() {
	RightAngleOptionDialog dlg;
	if (dlg.exec()) {
		glWidget->simplifyByRightAngle(dlg.getResolution(), dlg.getOptimization(), dlg.getLayeringThreshold(), dlg.getSnappingThreshold(), dlg.getOrientation() / 180.0 * CV_PI, dlg.getMinContourArea(), dlg.getMaxOBBRatio(), dlg.isAllowTriangleContour(), dlg.isAllowOverhang());
		glWidget->update();
	}
}

void MainWindow::onSimplifyByCurve() {
	CurveOptionDialog dlg;
	if (dlg.exec()) {
		glWidget->simplifyByCurve(dlg.getEpsilon(), dlg.getCurveThreshold(), dlg.getLayeringThreshold(), dlg.getSnappingThreshold(), dlg.getOrientation() / 180.0 * CV_PI, dlg.getMinContourArea(), dlg.getMaxOBBRatio(), dlg.isAllowTriangleContour(), dlg.isAllowOverhang());
		glWidget->update();
	}
}

void MainWindow::onSimplifyByCurveRightAngle() {
	CurveRightAngleOptionDialog dlg;
	if (dlg.exec()) {
		glWidget->simplifyByCurveRightAngle(dlg.getEpsilon(), dlg.getCurveThreshold(), dlg.getAngleThreshold() / 180.0 * CV_PI, dlg.getLayeringThreshold(), dlg.getSnappingThreshold(), dlg.getOrientation() / 180.0 * CV_PI, dlg.getMinContourArea(), dlg.getMaxOBBRatio(), dlg.isAllowTriangleContour(), dlg.isAllowOverhang());
		glWidget->update();
	}
}

void MainWindow::onSimplifyByEfficientRANSAC() {
	EfficientRANSACOptionDialog dlg;
	if (dlg.exec()) {
		glWidget->simplifyByEfficientRansac(dlg.getCurveNumIterations(), dlg.getCurveMinPoints(), dlg.getCurveMaxErrorRatioToRadius(), dlg.getCurveClusterEpsilon(), dlg.getCurveMinAngle() / 180.0 * CV_PI, dlg.getCurveMinRadius(), dlg.getCurveMaxRadius(), dlg.getLineNumIterations(), dlg.getLineMinPoints() * 0.01, dlg.getLineMaxError(), dlg.getLineClusterEpsilon() * 0.01, dlg.getLineMinLength() * 0.01, dlg.getLineAngleThreshold() / 180.0 * CV_PI, dlg.getContourMaxError(), dlg.getContourAngleThreshold() / 180.0 * CV_PI, dlg.getLayeringThreshold(), dlg.getSnappingThreshold(), dlg.getOrientation() / 180.0 * CV_PI, dlg.getMinContourArea(), dlg.getMaxOBBRatio(), dlg.isAllowTriangleContour(), dlg.isAllowOverhang(), dlg.getConfigFile());
	}
}

void MainWindow::onOffsetScale() {
	OffsetScaleDialog dlg;
	dlg.setOffset(glWidget->offset.x, glWidget->offset.y, glWidget->offset.z);
	dlg.setScale(glWidget->scale);
	if (dlg.exec()) {
		glWidget->offset = glm::dvec3(dlg.getOffsetX(), dlg.getOffsetY(), dlg.getOffsetZ());
		glWidget->scale = dlg.getScale();
		glWidget->update3DGeometry();
		glWidget->update();
	}
}

void MainWindow::onColoringModeChanged() {
	if (ui.actionColor->isChecked()) {
		glWidget->color_mode = GLWidget3D::COLOR;
	}
	else if (ui.actionTexture->isChecked()) {
		glWidget->color_mode = GLWidget3D::TEXTURE;
	}
	glWidget->update3DGeometry();
	glWidget->update();
}

void MainWindow::onRenderingModeChanged() {
	if (ui.actionRenderingBasic->isChecked()) {
		glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_BASIC;
	}
	else if (ui.actionRenderingSSAO->isChecked()) {
		glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_SSAO;
	}
	else if (ui.actionRenderingHatching->isChecked()) {
		glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_HATCHING;
	}
	glWidget->update();
}

void MainWindow::onGenerateFacadeImages(){
	GenerateFacadeOptionDialog dlg;
	if (dlg.exec()) {
		if (dlg.isAllowDoors())
			glWidget->generateFacadeImages(dlg.getPathForFacadeImages(), dlg.getNumberOfImages(), dlg.isAllowDataAugmentaion(), dlg.getWidth(), dlg.getHeight(), dlg.getNR(), dlg.getNC(), dlg.getNG(), dlg.getRelativeWidth(), dlg.getRelativeHeight(), dlg.isAllowWindowDisplacement(), dlg.getWindowDisplacement(), dlg.isAllowWindowProb(), dlg.getWindowProb(), dlg.isAllowPadding(), dlg.getPadding(), dlg.getND(), dlg.getDRelativeWidth(), dlg.getDRelativeHeight());
		else
			glWidget->generateFacadeImages(dlg.getPathForFacadeImages(), dlg.getNumberOfImages(), dlg.isAllowDataAugmentaion(), dlg.getWidth(), dlg.getHeight(), dlg.getNR(), dlg.getNC(), dlg.getNG(), dlg.getRelativeWidth(), dlg.getRelativeHeight(), dlg.isAllowWindowDisplacement(), dlg.getWindowDisplacement(), dlg.isAllowWindowProb(), dlg.getWindowProb(), dlg.isAllowPadding(), dlg.getPadding());
	}
}

void MainWindow::onGenerateRectifiedImage() {
	{
		int img_rows = 5;
		int img_cols = 5;
		int img_groups = 2;
		double relative_widht = 0.5;
		double relative_height = 0.5;
		cv::Mat final_img = glWidget->generateFacadeSynImage(224, 224, img_rows, img_cols, img_groups, relative_widht, relative_height);
		cv::imwrite("../data/test.png", final_img);
	}
	return;
	//
	QString filename = QFileDialog::getOpenFileName(this, tr("Load ..."), "", tr("Image files (*.png *.jpg *.bmp)"));
	if (filename.isEmpty()) return;
	cv::Scalar bg_color(255, 255, 255); // white back ground
	cv::Scalar window_color(0, 0, 0); // black for windows
	cv::Mat src = cv::imread(filename.toUtf8().constData(), 0);
	// scale to NN image size
	int height = 224;
	int width = 224;
	cv::Mat scale_img;
	cv::resize(src, scale_img, cv::Size(width, height));
	// Find contours
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(scale_img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	// Approximate contours to polygons + get bounding rects and circles
	std::vector<cv::Rect> boundRect(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
	}

	// Draw polygonal contour + bonding rects + circles
	cv::Mat drawing(scale_img.size(), CV_8UC3, bg_color);
	for (int i = 0; i< contours.size(); i++)
	{
		//cv::drawContours(drawing, contours, i, window_color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
		if (hierarchy[i][2] != -1) continue;
		cv::rectangle(drawing, cv::Point(boundRect[i].tl().x + 1, boundRect[i].tl().y + 1), cv::Point(boundRect[i].br().x, boundRect[i].br().y), window_color, -1);
	}
	int pos = filename.lastIndexOf(".");
	QString result_name = filename.left(pos) + "_output.png";
	std::cout << "result_name is " << result_name.toUtf8().constData() << std::endl;
	cv::cvtColor(scale_img, scale_img, cv::COLOR_GRAY2BGR);
	cv::imwrite(result_name.toUtf8().constData(), scale_img);
	return;
	// get synthetic facade image based on parameters
	cv::Mat final_img;
	{
		std::vector<double> paras;

		// v2
		/*paras.push_back(0.5622);
		paras.push_back(0.951);
		paras.push_back(0.6858);
		paras.push_back(0.5022);*/

		/*paras.push_back(0.5822);
		paras.push_back(0.4022);
		paras.push_back(0.5714);
		paras.push_back(0.491);*/

		/*paras.push_back(0.3488);
		paras.push_back(0.1879);
		paras.push_back(0.6395);
		paras.push_back(0.624);*/

		/*paras.push_back(0.4253);
		paras.push_back(0.0288);
		paras.push_back(0.7289);
		paras.push_back(0.5789);*/

		/*paras.push_back(0.5722);
		paras.push_back(0.5306);
		paras.push_back(0.5488);
		paras.push_back(0.3768);*/

		/*paras.push_back(0.2806);
		paras.push_back(0.3817);
		paras.push_back(0.5031);
		paras.push_back(0.5382);*/


		// v1
		/*paras.push_back(0.4915);
		paras.push_back(0.8496);
		paras.push_back(0.5301);
		paras.push_back(0.4905);*/

		/*paras.push_back(0.8152);
		paras.push_back(0.5340);
		paras.push_back(0.5134);
		paras.push_back(0.5675);*/

		/*paras.push_back(0.4287);
		paras.push_back(0.3372);
		paras.push_back(0.5442);
		paras.push_back(0.5423);*/

		/*paras.push_back(0.3502);
		paras.push_back(0.5533);
		paras.push_back(0.6352);
		paras.push_back(0.6255);*/

		/*paras.push_back(0.6687);
		paras.push_back(0.5243);
		paras.push_back(0.5373);
		paras.push_back(0.463);*/

		paras.push_back(0.4166);
		paras.push_back(0.4558);
		paras.push_back(0.01);
		paras.push_back(0.4780);
		paras.push_back(0.5248);

		std::pair<int, int> imageRows(4, 13);
		std::pair<int, int> imageCols(4, 13);
		std::pair<int, int> imageGroups(1, 3);
		int img_rows = round(paras[0] * (imageRows.second - imageRows.first) + imageRows.first);
		int img_cols = round(paras[1] * (imageCols.second - imageCols.first) + imageCols.first);
		int img_groups = paras[2] * (imageGroups.second - imageGroups.first) + imageGroups.first;
		double relative_widht = paras[3];
		double relative_height = paras[4];

		final_img = glWidget->generateFacadeSynImage(width, height, img_rows, img_cols, img_groups, relative_widht, relative_height);
	}
	// recover to the original image
	cv::resize(final_img, final_img, src.size());
	QString final_name = filename.left(pos) + "_final.png";
	cv::imwrite(final_name.toUtf8().constData(), final_img);
}