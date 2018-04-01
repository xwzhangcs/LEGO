#include "PLYOptionDialog.h"
#include <QFileDialog>

PLYOptionDialog::PLYOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.lineEditOffsetX->setText("0");
	ui.lineEditOffsetY->setText("0");
	ui.lineEditOffsetZ->setText("0");
	ui.doubleSpinBoxScale->setValue(0.3);
	ui.doubleSpinBoxScale->setSingleStep(0.1);
	ui.doubleSpinBoxScale->setMinimum(0.1);
	ui.doubleSpinBoxScale->setMaximum(10.0);
	ui.lineEditFileName->setText("buildings.ply");

	connect(ui.pushButtonD1, SIGNAL(clicked()), this, SLOT(onD1()));
	connect(ui.pushButtonD2, SIGNAL(clicked()), this, SLOT(onD2()));
	connect(ui.pushButtonD3, SIGNAL(clicked()), this, SLOT(onD3()));
	connect(ui.pushButtonD4, SIGNAL(clicked()), this, SLOT(onD4()));
	connect(ui.pushButtonFileName, SIGNAL(clicked()), this, SLOT(onFileName()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

PLYOptionDialog::~PLYOptionDialog() {
}

void PLYOptionDialog::onD1() {
	ui.lineEditOffsetX->setText("0");
	ui.lineEditOffsetY->setText("0");
}

void PLYOptionDialog::onD2() {
	ui.lineEditOffsetX->setText("749349.916482");
	ui.lineEditOffsetY->setText("4407879.580789001");
}

void PLYOptionDialog::onD3() {
	ui.lineEditOffsetX->setText("0");
	ui.lineEditOffsetY->setText("0");
}

void PLYOptionDialog::onD4() {
	ui.lineEditOffsetX->setText("435510.093075");
	ui.lineEditOffsetY->setText("3355541.032361");
}

double PLYOptionDialog::getOffsetX() {
	return ui.lineEditOffsetX->text().toDouble();
}

double PLYOptionDialog::getOffsetY() {
	return ui.lineEditOffsetY->text().toDouble();
}

double PLYOptionDialog::getOffsetZ() {
	return ui.lineEditOffsetZ->text().toDouble();
}

double PLYOptionDialog::getScale() {
	return ui.doubleSpinBoxScale->value();
}

QString PLYOptionDialog::getFileName() {
	return ui.lineEditFileName->text();
}

void PLYOptionDialog::onFileName() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save PLY file..."), "", tr("PLY files (*.ply)"));
	if (!filename.isEmpty()) {
		ui.lineEditFileName->setText(filename);
	}
}

void PLYOptionDialog::onOK() {
	accept();
}

void PLYOptionDialog::onCancel() {
	reject();
}
