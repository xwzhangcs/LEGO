#include "GenerateFacadeOptionDialog.h"
#include <QFileDialog>

GenerateFacadeOptionDialog::GenerateFacadeOptionDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.lineEditImageHeight->setText("220");
	ui.lineEditImageWidth->setText("220");
	ui.lineEditImageNum->setText("1");
	ui.lineEditNCMin->setText("4");
	ui.lineEditNCMax->setText("13");
	ui.lineEditNRMin->setText("4");
	ui.lineEditNRMax->setText("13");
	ui.lineEditNGMin->setText("1");
	ui.lineEditNGMax->setText("3");
	ui.lineEditNGMin->setText("1");
	ui.lineEditRelativeWidthMin->setText("0.55");
	ui.lineEditRelativeWidthMax->setText("0.85");
	ui.lineEditRelativeHeightMin->setText("0.55");
	ui.lineEditRelativeHeightMax->setText("0.85");
	ui.checkBoxWindowDis->setChecked(false);
	ui.lineEditWindowDis->setText("0.15");
	ui.checkBoxWindowProb->setChecked(false);
	ui.lineEditWindowProb->setText("0.85");
	ui.checkBoxDataAug->setChecked(false);
	ui.checkBoxPadding->setChecked(false);
	ui.lineEditPaddingMin->setText("2");
	ui.lineEditPaddingMax->setText("8");

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
	connect(ui.pushButtonInputPath, SIGNAL(clicked()), this, SLOT(onGetPath()));
	connect(ui.checkBoxWindowDis, SIGNAL(clicked()), this, SLOT(onUseWindowDis()));
	connect(ui.checkBoxWindowProb, SIGNAL(clicked()), this, SLOT(onUseWindowProb()));
	connect(ui.checkBoxPadding, SIGNAL(clicked()), this, SLOT(onUsePadding()));

	onUseWindowDis();
	onUseWindowProb();
	onUsePadding();
}

GenerateFacadeOptionDialog::~GenerateFacadeOptionDialog()
{
}

QString GenerateFacadeOptionDialog::getPathForFacadeImages(){
	return ui.lineEditFacadeImagePath->text();
}

int GenerateFacadeOptionDialog::getNumberOfImages(){
	return ui.lineEditImageNum->text().toInt();
}

std::pair<int, int> GenerateFacadeOptionDialog::getNR(){
	return std::make_pair(ui.lineEditNRMin->text().toInt(), ui.lineEditNRMax->text().toInt());
}

std::pair<int, int> GenerateFacadeOptionDialog::getNC(){
	return std::make_pair(ui.lineEditNCMin->text().toInt(), ui.lineEditNCMax->text().toInt());
}

std::pair<int, int> GenerateFacadeOptionDialog::getNG(){
	return std::make_pair(ui.lineEditNGMin->text().toInt(), ui.lineEditNGMax->text().toInt());
}

int GenerateFacadeOptionDialog::getWidth(){
	return ui.lineEditImageWidth->text().toInt();
}

int GenerateFacadeOptionDialog::getHeight(){
	return ui.lineEditImageHeight->text().toInt();
}

std::pair<double, double> GenerateFacadeOptionDialog::getRelativeWidth(){
	return std::make_pair(ui.lineEditRelativeWidthMin->text().toDouble(), ui.lineEditRelativeWidthMax->text().toDouble());
}

std::pair<double, double> GenerateFacadeOptionDialog::getRelativeHeight(){
	return std::make_pair(ui.lineEditRelativeHeightMin->text().toDouble(), ui.lineEditRelativeHeightMax->text().toDouble());
}

bool GenerateFacadeOptionDialog::isAllowWindowDisplacement(){
	return ui.checkBoxWindowDis->isChecked();
}

double GenerateFacadeOptionDialog::getWindowDisplacement(){
	return ui.lineEditWindowDis->text().toDouble();
}

bool GenerateFacadeOptionDialog::isAllowWindowProb(){
	return ui.checkBoxWindowProb->isChecked();
}

bool GenerateFacadeOptionDialog::isAllowPadding(){
	return ui.checkBoxPadding->isChecked();
}

std::pair<int, int> GenerateFacadeOptionDialog::getPadding(){
	return std::make_pair(ui.lineEditPaddingMin->text().toInt(), ui.lineEditPaddingMax->text().toInt());
}

double GenerateFacadeOptionDialog::getWindowProb(){
	return ui.lineEditWindowProb->text().toDouble();
}

bool GenerateFacadeOptionDialog::isAllowDataAugmentaion(){
	return ui.checkBoxDataAug->isChecked();
}

void GenerateFacadeOptionDialog::onOK() {
	accept();
}

void GenerateFacadeOptionDialog::onCancel() {
	reject();
}

void GenerateFacadeOptionDialog::onGetPath() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose path..."), ui.lineEditFacadeImagePath->text());
	if (!dir.isEmpty()) {
		ui.lineEditFacadeImagePath->setText(dir);
	}
}

void GenerateFacadeOptionDialog::onUseWindowDis() {
	ui.lineEditWindowDis->setEnabled(ui.checkBoxWindowDis->isChecked());
}

void GenerateFacadeOptionDialog::onUseWindowProb() {
	ui.lineEditWindowProb->setEnabled(ui.checkBoxWindowProb->isChecked());
}

void GenerateFacadeOptionDialog::onUsePadding() {
	ui.lineEditPaddingMin->setEnabled(ui.checkBoxPadding->isChecked());
	ui.lineEditPaddingMax->setEnabled(ui.checkBoxPadding->isChecked());
	ui.label_19->setEnabled(ui.checkBoxPadding->isChecked());
}