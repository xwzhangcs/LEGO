#pragma once

#include <QDialog>
#include "ui_GenerateRoofOptionDialog.h"

class GenerateRoofOptionDialog : public QDialog
{
	Q_OBJECT

public:
	GenerateRoofOptionDialog(QWidget *parent = Q_NULLPTR);
	~GenerateRoofOptionDialog();
	QString getPathForRoofImages();
	int getNumberOfImages();
	int getRoofType();
	int getWidth();
	int getHeight();
	std::pair<int, int> getRoofWidth();
	std::pair<double, double> getAspect();
	std::pair<double, double> getSlope();
	std::pair<double, double> getRidge();
	bool isAllowHeightDisplacement();
	double getHeightDisplacement();
	bool isAllowDataAugmentaion();

private:
	Ui::GenerateRoofOptionDialog ui;

public slots:
	void onOK();
	void onCancel();
	void onGetPath();
	void onUseHeightDis();
	void onUseRoofType();
};
