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
	void onSavePLY();
	void onSaveImage();
	void onInputVoxel();
	void onSimplifyByAll();
	void onSimplifyByDP();
	void onSimplifyByRightAngle();
	void onSimplifyByCurve();
	void onOpenCVTest();
	void onRightAngleTest();
	void onCurveTest();
	void onColoringModeChanged();
	void onRenderingModeChanged();
};

#endif // MAINWINDOW_H
