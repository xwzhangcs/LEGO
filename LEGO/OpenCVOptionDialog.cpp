#include "OpenCVOptionDialog.h"

OpenCVOptionDialog::OpenCVOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.doubleSpinBoxEpsilon->setValue(1.0);

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

OpenCVOptionDialog::~OpenCVOptionDialog() {
}

double OpenCVOptionDialog::getEpsilon() {
	return ui.doubleSpinBoxEpsilon->value();
}

void OpenCVOptionDialog::onOK() {
	accept();
}

void OpenCVOptionDialog::onCancel() {
	reject();
}
