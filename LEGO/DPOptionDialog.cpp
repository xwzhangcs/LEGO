#include "DPOptionDialog.h"

DPOptionDialog::DPOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.spinBoxEpsilon->setValue(16);
	ui.doubleSpinBoxLayeringThreshold->setValue(0.7);
	ui.doubleSpinBoxLayeringThreshold->setSingleStep(0.1);
	ui.doubleSpinBoxLayeringThreshold->setMinimum(0.0);
	ui.doubleSpinBoxLayeringThreshold->setMaximum(1.0);
	ui.doubleSpinBoxSnappingThreshold->setValue(1.0);
	ui.lineEditOrientation->setText("0");

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

DPOptionDialog::~DPOptionDialog() {
}

int DPOptionDialog::getEpsilon() {
	return ui.spinBoxEpsilon->value();
}

double DPOptionDialog::getLayeringThreshold() {
	return ui.doubleSpinBoxLayeringThreshold->value();
}

double DPOptionDialog::getSnappingThreshold() {
	return ui.doubleSpinBoxSnappingThreshold->value();
}

double DPOptionDialog::getOrientation() {
	return ui.lineEditOrientation->text().toDouble();
}

void DPOptionDialog::onOK() {
	accept();
}

void DPOptionDialog::onCancel() {
	reject();
}
