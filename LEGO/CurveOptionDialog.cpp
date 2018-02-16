#include "CurveOptionDialog.h"

CurveOptionDialog::CurveOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.spinBoxEpsilon->setValue(2);
	ui.doubleSpinBoxCurveThreshold->setValue(1.0);
	ui.doubleSpinBoxLayeringThreshold->setValue(0.1);
	ui.doubleSpinBoxLayeringThreshold->setSingleStep(0.1);
	ui.doubleSpinBoxSnapVertexThreshold->setValue(1.0);
	ui.doubleSpinBoxSnapEdgeThreshold->setValue(1.0);

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

double CurveOptionDialog::getSnapVertexThreshold() {
	return ui.doubleSpinBoxSnapVertexThreshold->value();
}

double CurveOptionDialog::getSnapEdgeThreshold() {
	return ui.doubleSpinBoxSnapEdgeThreshold->value();
}

void CurveOptionDialog::onOK() {
	accept();
}

void CurveOptionDialog::onCancel() {
	reject();
}
