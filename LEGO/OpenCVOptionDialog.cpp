#include "OpenCVOptionDialog.h"

OpenCVOptionDialog::OpenCVOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.spinBoxEpsilon->setValue(1);
	ui.doubleSpinBoxLayeringThreshold->setValue(0.8);
	ui.doubleSpinBoxLayeringThreshold->setSingleStep(0.1);
	ui.doubleSpinBoxSnapVertexThreshold->setValue(1.0);
	ui.doubleSpinBoxSnapEdgeThreshold->setValue(1.0);

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

OpenCVOptionDialog::~OpenCVOptionDialog() {
}

int OpenCVOptionDialog::getEpsilon() {
	return ui.spinBoxEpsilon->value();
}

double OpenCVOptionDialog::getLayeringThreshold() {
	return ui.doubleSpinBoxLayeringThreshold->value();
}

double OpenCVOptionDialog::getSnapVertexThreshold() {
	return ui.doubleSpinBoxSnapVertexThreshold->value();
}

double OpenCVOptionDialog::getSnapEdgeThreshold() {
	return ui.doubleSpinBoxSnapEdgeThreshold->value();
}

void OpenCVOptionDialog::onOK() {
	accept();
}

void OpenCVOptionDialog::onCancel() {
	reject();
}
