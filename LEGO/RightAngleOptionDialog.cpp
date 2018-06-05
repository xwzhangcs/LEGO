#include "RightAngleOptionDialog.h"

RightAngleOptionDialog::RightAngleOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.spinBoxResolution->setValue(20);
	ui.doubleSpinBoxLayeringThreshold->setValue(0.7);
	ui.doubleSpinBoxLayeringThreshold->setSingleStep(0.1);
	ui.doubleSpinBoxLayeringThreshold->setMinimum(0.0);
	ui.doubleSpinBoxLayeringThreshold->setMaximum(1.0);
	ui.doubleSpinBoxSnappingThreshold->setValue(1.0);
	ui.lineEditOrientation->setText("0");

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

RightAngleOptionDialog::~RightAngleOptionDialog() {
}

int RightAngleOptionDialog::getResolution() {
	return ui.spinBoxResolution->value();
}

double RightAngleOptionDialog::getLayeringThreshold() {
	return ui.doubleSpinBoxLayeringThreshold->value();
}

double RightAngleOptionDialog::getSnappingThreshold() {
	return ui.doubleSpinBoxSnappingThreshold->value();
}

double RightAngleOptionDialog::getOrientation() {
	return ui.lineEditOrientation->text().toDouble();
}

void RightAngleOptionDialog::onOK() {
	accept();
}

void RightAngleOptionDialog::onCancel() {
	reject();
}
