#include "RightAngleOptionDialog.h"

RightAngleOptionDialog::RightAngleOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.spinBoxResolution->setValue(4);
	ui.doubleSpinBoxLayeringThreshold->setValue(0.1);
	ui.doubleSpinBoxLayeringThreshold->setSingleStep(0.1);
	ui.doubleSpinBoxLayeringThreshold->setMinimum(0.0);
	ui.doubleSpinBoxLayeringThreshold->setMaximum(1.0);
	ui.doubleSpinBoxSnapVertexThreshold->setValue(1.0);
	ui.doubleSpinBoxSnapEdgeThreshold->setValue(1.0);

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

double RightAngleOptionDialog::getSnapVertexThreshold() {
	return ui.doubleSpinBoxSnapVertexThreshold->value();
}

double RightAngleOptionDialog::getSnapEdgeThreshold() {
	return ui.doubleSpinBoxSnapEdgeThreshold->value();
}

void RightAngleOptionDialog::onOK() {
	accept();
}

void RightAngleOptionDialog::onCancel() {
	reject();
}
