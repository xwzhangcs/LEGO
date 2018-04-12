#include "OBJOptionDialog.h"
#include <QFileDialog>

OBJOptionDialog::OBJOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.lineEditOffsetX->setText("0");
	ui.lineEditOffsetY->setText("0");
	ui.lineEditOffsetZ->setText("0");
	ui.doubleSpinBoxScale->setValue(0.3);
	ui.doubleSpinBoxScale->setSingleStep(0.1);
	ui.doubleSpinBoxScale->setMinimum(0.1);
	ui.doubleSpinBoxScale->setMaximum(10.0);
	ui.lineEditFileName->setText("buildings.obj");

	connect(ui.pushButtonD1, SIGNAL(clicked()), this, SLOT(onD1()));
	connect(ui.pushButtonD2, SIGNAL(clicked()), this, SLOT(onD2()));
	connect(ui.pushButtonD3, SIGNAL(clicked()), this, SLOT(onD3()));
	connect(ui.pushButtonD4, SIGNAL(clicked()), this, SLOT(onD4()));
	connect(ui.pushButtonFileName, SIGNAL(clicked()), this, SLOT(onFileName()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

OBJOptionDialog::~OBJOptionDialog() {
}

void OBJOptionDialog::onD1() {
	ui.lineEditOffsetX->setText("0");
	ui.lineEditOffsetY->setText("0");
}

void OBJOptionDialog::onD2() {
	ui.lineEditOffsetX->setText("749349.916482");
	ui.lineEditOffsetY->setText("4407879.580789001");
}

void OBJOptionDialog::onD3() {
	ui.lineEditOffsetX->setText("0");
	ui.lineEditOffsetY->setText("0");
}

void OBJOptionDialog::onD4() {
	ui.lineEditOffsetX->setText("435510.093075");
	ui.lineEditOffsetY->setText("3355541.032361");
}

double OBJOptionDialog::getOffsetX() {
	return ui.lineEditOffsetX->text().toDouble();
}

double OBJOptionDialog::getOffsetY() {
	return ui.lineEditOffsetY->text().toDouble();
}

double OBJOptionDialog::getOffsetZ() {
	return ui.lineEditOffsetZ->text().toDouble();
}

double OBJOptionDialog::getScale() {
	return ui.doubleSpinBoxScale->value();
}

QString OBJOptionDialog::getFileName() {
	return ui.lineEditFileName->text();
}

void OBJOptionDialog::onFileName() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save OBJ file..."), "", tr("OBJ files (*.obj)"));
	if (!filename.isEmpty()) {
		ui.lineEditFileName->setText(filename);
	}
}

void OBJOptionDialog::onOK() {
	accept();
}

void OBJOptionDialog::onCancel() {
	reject();
}
