#include "TopFaceOptionDialog.h"
#include <QFileDialog>

TopFaceOptionDialog::TopFaceOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.lineEditOffsetX->setText("0");
	ui.lineEditOffsetY->setText("0");
	ui.lineEditOffsetZ->setText("0");
	ui.doubleSpinBoxScale->setValue(0.3);
	ui.doubleSpinBoxScale->setSingleStep(0.1);
	ui.doubleSpinBoxScale->setMinimum(0.1);
	ui.doubleSpinBoxScale->setMaximum(10.0);
	ui.lineEditFileName->setText("topface.txt");

	connect(ui.pushButtonD1, SIGNAL(clicked()), this, SLOT(onD1()));
	connect(ui.pushButtonD2, SIGNAL(clicked()), this, SLOT(onD2()));
	connect(ui.pushButtonD3, SIGNAL(clicked()), this, SLOT(onD3()));
	connect(ui.pushButtonD4, SIGNAL(clicked()), this, SLOT(onD4()));
	connect(ui.pushButtonFileName, SIGNAL(clicked()), this, SLOT(onFileName()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

TopFaceOptionDialog::~TopFaceOptionDialog() {
}

void TopFaceOptionDialog::onD1() {
	ui.lineEditOffsetX->setText("0");
	ui.lineEditOffsetY->setText("0");
}

void TopFaceOptionDialog::onD2() {
	ui.lineEditOffsetX->setText("749349.916482");
	ui.lineEditOffsetY->setText("4407879.580789001");
}

void TopFaceOptionDialog::onD3() {
	ui.lineEditOffsetX->setText("0");
	ui.lineEditOffsetY->setText("0");
}

void TopFaceOptionDialog::onD4() {
	ui.lineEditOffsetX->setText("435510.093075");
	ui.lineEditOffsetY->setText("3355541.032361");
}

double TopFaceOptionDialog::getOffsetX() {
	return ui.lineEditOffsetX->text().toDouble();
}

double TopFaceOptionDialog::getOffsetY() {
	return ui.lineEditOffsetY->text().toDouble();
}

double TopFaceOptionDialog::getOffsetZ() {
	return ui.lineEditOffsetZ->text().toDouble();
}

double TopFaceOptionDialog::getScale() {
	return ui.doubleSpinBoxScale->value();
}

QString TopFaceOptionDialog::getFileName() {
	return ui.lineEditFileName->text();
}

void TopFaceOptionDialog::onFileName() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save text file..."), "", tr("text files (*.txt)"));
	if (!filename.isEmpty()) {
		ui.lineEditFileName->setText(filename);
	}
}

void TopFaceOptionDialog::onOK() {
	accept();
}

void TopFaceOptionDialog::onCancel() {
	reject();
}
