#include "MainWindow.h"
#include <QFileDialog>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	// group for rendering modes
	QActionGroup* groupRendering = new QActionGroup(this);
	groupRendering->addAction(ui.actionRenderingBasic);
	groupRendering->addAction(ui.actionRenderingSSAO);
	groupRendering->addAction(ui.actionRenderingHatching);

	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionRenderingBasic, SIGNAL(triggered()), this, SLOT(onRenderingModeChanged()));
	connect(ui.actionRenderingSSAO, SIGNAL(triggered()), this, SLOT(onRenderingModeChanged()));
	connect(ui.actionRenderingHatching, SIGNAL(triggered()), this, SLOT(onRenderingModeChanged()));

	// create tool bar for file menu
	ui.mainToolBar->addAction(ui.actionOpen);

	// setup the GL widget
	glWidget = new GLWidget3D(this);
	setCentralWidget(glWidget);
}

MainWindow::~MainWindow() {
}

void MainWindow::onOpen() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load voxel data..."), "", tr("Image files (*.png *.jpg *.bmp)"));
	if (filename.isEmpty()) return;

	// get directory
	QDir dir = QFileInfo(filename).absoluteDir();

	// scan all the files in the directory to get a voxel data
	QStringList files = dir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::DirsFirst);
	std::vector<cv::Mat> voxel_data(files.size());
	for (int i = 0; i < files.size(); i++) {
		//std::cout << "[" << files[i].toUtf8().constData() << "]" << std::endl;
		voxel_data[i] = cv::imread((dir.absolutePath() + "/" + files[i]).toUtf8().constData(), cv::IMREAD_GRAYSCALE);
	}

	glWidget->loadVoxelData(voxel_data);
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
}