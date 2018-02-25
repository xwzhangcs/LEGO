#include "OptionDialog.h"

OptionDialog::OptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.doubleSpinBoxAlpha->setValue(0.5);
	ui.doubleSpinBoxAlpha->setSingleStep(0.1);

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

OptionDialog::~OptionDialog() {
}

double OptionDialog::getAlpha() {
	return ui.doubleSpinBoxAlpha->value();
}

void OptionDialog::onOK() {
	accept();
}

void OptionDialog::onCancel() {
	reject();
}
