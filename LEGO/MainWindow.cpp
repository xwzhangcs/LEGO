#include "MainWindow.h"
#include <iostream>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>
#include "OpenCVOptionDialog.h"
#include "OurCustomOptionDialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	// group for rendering modes
	QActionGroup* groupColoring = new QActionGroup(this);
	groupColoring->addAction(ui.actionSameColor);
	groupColoring->addAction(ui.actionColoringByBuilding);
	groupColoring->addAction(ui.actionColoringByLayer);

	// group for rendering modes
	QActionGroup* groupRendering = new QActionGroup(this);
	groupRendering->addAction(ui.actionRenderingBasic);
	groupRendering->addAction(ui.actionRenderingSSAO);
	groupRendering->addAction(ui.actionRenderingHatching);

	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionSavePLY, SIGNAL(triggered()), this, SLOT(onSavePLY()));
	connect(ui.actionSaveImage, SIGNAL(triggered()), this, SLOT(onSaveImage()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionInputVoxel, SIGNAL(triggered()), this, SLOT(onInputVoxel()));
	connect(ui.actionSimplifyByOpenCV, SIGNAL(triggered()), this, SLOT(onSimplifyByOpenCV()));
	connect(ui.actionSimplifyByOurCustom, SIGNAL(triggered()), this, SLOT(onSimplifyByOurCustom()));
	connect(ui.actionSameColor, SIGNAL(triggered()), this, SLOT(onColoringModeChanged()));
	connect(ui.actionColoringByBuilding, SIGNAL(triggered()), this, SLOT(onColoringModeChanged()));
	connect(ui.actionColoringByLayer, SIGNAL(triggered()), this, SLOT(onColoringModeChanged()));
	connect(ui.actionRenderingBasic, SIGNAL(triggered()), this, SLOT(onRenderingModeChanged()));
	connect(ui.actionRenderingSSAO, SIGNAL(triggered()), this, SLOT(onRenderingModeChanged()));
	connect(ui.actionRenderingHatching, SIGNAL(triggered()), this, SLOT(onRenderingModeChanged()));

	// create tool bar for file menu
	ui.mainToolBar->addAction(ui.actionOpen);
	ui.mainToolBar->addAction(ui.actionSavePLY);

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

void MainWindow::onSavePLY() {
	if (glWidget->show_mode == GLWidget3D::SHOW_INPUT) {
		QMessageBox msg;
		msg.setText("Simplify the buildings.");
		msg.exec();
		return;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Save PLY file..."), "", tr("PLY files (*.ply)"));
	if (filename.isEmpty()) return;

	glWidget->savePLY(filename);
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

void MainWindow::onSimplifyByOpenCV() {
	OpenCVOptionDialog dlg;
	if (dlg.exec()) {
		glWidget->simplifyByOpenCV(dlg.getEpsilon(), dlg.getLayeringThreshold(), dlg.getSnapVertexThreshold(), dlg.getSnapEdgeThreshold());
		glWidget->update();
	}
}

void MainWindow::onSimplifyByOurCustom() {
	OurCustomOptionDialog dlg;
	if (dlg.exec()) {
		glWidget->simplifyByOurCustom(dlg.getResolution(), dlg.getLayeringThreshold(), dlg.getSnapVertexThreshold(), dlg.getSnapEdgeThreshold());
		glWidget->update();
	}
}

void MainWindow::onColoringModeChanged() {
	if (ui.actionSameColor->isChecked()) {
		glWidget->color_mode = GLWidget3D::COLOR_SAME;
	}
	else if (ui.actionColoringByBuilding->isChecked()) {
		glWidget->color_mode = GLWidget3D::COLOR_BY_BUILDING;
	}
	else if (ui.actionColoringByLayer->isChecked()) {
		glWidget->color_mode = GLWidget3D::COLOR_BY_LAYER;
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