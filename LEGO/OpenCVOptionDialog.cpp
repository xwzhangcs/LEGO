#include "OpenCVOptionDialog.h"

OpenCVOptionDialog::OpenCVOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.spinBoxEpsilon->setValue(1);
	ui.doubleSpinBoxSlicingThreshold->setValue(0.7);

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

OpenCVOptionDialog::~OpenCVOptionDialog() {
}

int OpenCVOptionDialog::getEpsilon() {
	return ui.spinBoxEpsilon->value();
}

double OpenCVOptionDialog::getSlicingThreshold() {
	return ui.doubleSpinBoxSlicingThreshold->value();
}

void OpenCVOptionDialog::onOK() {
	accept();
}

void OpenCVOptionDialog::onCancel() {
	reject();
}
