#include "GenerateRoofOptionDialog.h"
#include <QFileDialog>

GenerateRoofOptionDialog::GenerateRoofOptionDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.lineEditImageHeight->setText("224");
	ui.lineEditImageWidth->setText("224");
	ui.lineEditImageNum->setText("1");
	ui.lineEditWidthMin->setText("1");
	ui.lineEditWidthMax->setText("1");
	ui.lineEditAspectMin->setText("0.5");
	ui.lineEditAspectMax->setText("0.8");
	ui.lineEditSlopeMin->setText("30");
	ui.lineEditSlopeMax->setText("60");
	ui.lineEditRidgeMin->setText("0.4");
	ui.lineEditRidgeMax->setText("0.8");
	ui.checkBoxDataAug->setChecked(false);
	ui.comboBoxRoofTypes->setCurrentIndex(0);

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
	connect(ui.pushButtonInputPath, SIGNAL(clicked()), this, SLOT(onGetPath()));
	connect(ui.checkBoxHeightDis, SIGNAL(clicked()), this, SLOT(onUseHeightDis()));
	connect(ui.comboBoxRoofTypes, SIGNAL(currentIndexChanged(int)), this, SLOT(onUseRoofType()));

	onUseHeightDis();
	onUseRoofType();
}

GenerateRoofOptionDialog::~GenerateRoofOptionDialog()
{
}

void GenerateRoofOptionDialog::onOK() {
	accept();
}

void GenerateRoofOptionDialog::onCancel() {
	reject();
}

void GenerateRoofOptionDialog::onGetPath() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose path..."), ui.lineEditRoofImagePath->text());
	if (!dir.isEmpty()) {
		ui.lineEditRoofImagePath->setText(dir);
	}
}

void GenerateRoofOptionDialog::onUseHeightDis() {
	ui.lineEditHeightDis->setEnabled(ui.checkBoxHeightDis->isChecked());
}

void GenerateRoofOptionDialog::onUseRoofType(){
	if (ui.comboBoxRoofTypes->currentIndex() == 0){
		ui.lineEditSlopeMin->setEnabled(false);
		ui.lineEditSlopeMax->setEnabled(false);
		ui.label_13->setEnabled(false);
		ui.lineEditRidgeMin->setEnabled(false);
		ui.lineEditRidgeMax->setEnabled(false);
		ui.label_18->setEnabled(false);
	}
	else if (ui.comboBoxRoofTypes->currentIndex() == 1 || ui.comboBoxRoofTypes->currentIndex() == 3){
		ui.lineEditSlopeMin->setEnabled(true);
		ui.lineEditSlopeMax->setEnabled(true);
		ui.label_13->setEnabled(true);
		ui.lineEditRidgeMin->setEnabled(false);
		ui.lineEditRidgeMax->setEnabled(false);
		ui.label_18->setEnabled(false);
		
	}
	else if (ui.comboBoxRoofTypes->currentIndex() == 2){
		ui.lineEditSlopeMin->setEnabled(true);
		ui.lineEditSlopeMax->setEnabled(true);
		ui.label_13->setEnabled(true);
		ui.lineEditRidgeMin->setEnabled(true);
		ui.lineEditRidgeMax->setEnabled(true);
		ui.label_18->setEnabled(true);
	}
	else{

	}
}