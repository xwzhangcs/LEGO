#pragma once

#include <QDialog>
#include "ui_GenerateFacadeOptionDialog.h"

class GenerateFacadeOptionDialog : public QDialog
{
	Q_OBJECT

public:
	GenerateFacadeOptionDialog(QWidget *parent = Q_NULLPTR);
	~GenerateFacadeOptionDialog();
	QString getPathForFacadeImages();
	int getNumberOfImages();
	std::pair<int, int> getNR();
	std::pair<int, int> getNC();
	std::pair<int, int> getNG();
	int getWidth();
	int getHeight();
	std::pair<double, double> getRelativeWidth();
	std::pair<double, double> getRelativeHeight();
	bool isAllowWindowDisplacement();
	double getWindowDisplacement();
	bool isAllowWindowProb();
	double getWindowProb();

private:
	Ui::GenerateFacadeOptionDialog ui;

public slots:
	void onOK();
	void onCancel();
	void onGetPath();
	void onUseWindowDis();
	void onUseWindowProb();
};
