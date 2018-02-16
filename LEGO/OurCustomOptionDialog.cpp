#include "OurCustomOptionDialog.h"

OurCustomOptionDialog::OurCustomOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.spinBoxResolution->setValue(4);
	ui.doubleSpinBoxLayeringThreshold->setValue(0.1);
	ui.doubleSpinBoxLayeringThreshold->setSingleStep(0.1);
	ui.doubleSpinBoxSnapVertexThreshold->setValue(1.0);
	ui.doubleSpinBoxSnapEdgeThreshold->setValue(1.0);

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

OurCustomOptionDialog::~OurCustomOptionDialog() {
}

int OurCustomOptionDialog::getResolution() {
	return ui.spinBoxResolution->value();
}

double OurCustomOptionDialog::getLayeringThreshold() {
	return ui.doubleSpinBoxLayeringThreshold->value();
}

double OurCustomOptionDialog::getSnapVertexThreshold() {
	return ui.doubleSpinBoxSnapVertexThreshold->value();
}

double OurCustomOptionDialog::getSnapEdgeThreshold() {
	return ui.doubleSpinBoxSnapEdgeThreshold->value();
}

void OurCustomOptionDialog::onOK() {
	accept();
}

void OurCustomOptionDialog::onCancel() {
	reject();
}
