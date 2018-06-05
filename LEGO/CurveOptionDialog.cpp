#include "CurveOptionDialog.h"

CurveOptionDialog::CurveOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.spinBoxEpsilon->setValue(16);
	ui.doubleSpinBoxCurveThreshold->setValue(2.0);
	ui.doubleSpinBoxCurveThreshold->setSingleStep(0.1);
	ui.doubleSpinBoxLayeringThreshold->setValue(0.7);
	ui.doubleSpinBoxLayeringThreshold->setSingleStep(0.1);
	ui.doubleSpinBoxLayeringThreshold->setMinimum(0.0);
	ui.doubleSpinBoxLayeringThreshold->setMaximum(1.0);

	ui.doubleSpinBoxSnappingThreshold->setValue(1.0);
	ui.lineEditOrientation->setText("0");

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

CurveOptionDialog::~CurveOptionDialog() {
}

int CurveOptionDialog::getEpsilon() {
	return ui.spinBoxEpsilon->value();
}

double CurveOptionDialog::getCurveThreshold() {
	return ui.doubleSpinBoxCurveThreshold->value();
}

double CurveOptionDialog::getLayeringThreshold() {
	return ui.doubleSpinBoxLayeringThreshold->value();
}

double CurveOptionDialog::getSnappingThreshold() {
	return ui.doubleSpinBoxSnappingThreshold->value();
}

double CurveOptionDialog::getOrientation() {
	return ui.lineEditOrientation->text().toDouble();
}

void CurveOptionDialog::onOK() {
	accept();
}

void CurveOptionDialog::onCancel() {
	reject();
}
