#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "GLWidget3D.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

private:
	Ui::MainWindowClass ui;
	GLWidget3D* glWidget;

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();


public slots:
	void onOpen();
	void onSaveOBJ();
	void onSaveXYZN();
	void onSaveTopFaces();
	void onSavePLY();
	void onSaveImage();
	void onInputVoxel();
	void onSimplifyByAll();
	void onSimplifyByDP();
	void onSimplifyByRightAngle();
	void onSimplifyByCurve();
	void onSimplifyByCurveRightAngle();
	void onSimplifyByEfficientRANSAC();
	void onOffsetScale();
	void onColoringModeChanged();
	void onRenderingModeChanged();
	void onGenerateFacadeImages();
	void onGenerateRectifiedImage();
	void onGenerateRoofImages();
	void onGenerateEDImages();
	void onGenerateDeformImages();
	void onGenerateFuseImages();
};

#endif // MAINWINDOW_H
