#include "GenerateFacadeOptionDialog.h"
#include <QFileDialog>

GenerateFacadeOptionDialog::GenerateFacadeOptionDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.lineEditImageHeight->setText("118");
	ui.lineEditImageWidth->setText("118");
	ui.lineEditImageNum->setText("1");
	ui.lineEditNCMin->setText("3");
	ui.lineEditNCMax->setText("8");
	ui.lineEditNRMin->setText("3");
	ui.lineEditNRMax->setText("8");
	ui.lineEditNGMin->setText("1");
	ui.lineEditNGMax->setText("1");
	ui.lineEditRelativeWidthMin->setText("0.4");
	ui.lineEditRelativeWidthMax->setText("0.7");
	ui.lineEditRelativeHeightMin->setText("0.4");
	ui.lineEditRelativeHeightMax->setText("0.7");
	ui.checkBoxWindowDis->setChecked(false);
	ui.lineEditWindowDis->setText("0.06");
	ui.checkBoxWindowProb->setChecked(false);
	ui.lineEditWindowProb->setText("0.7");
	ui.checkBoxDataAug->setChecked(false);
	ui.checkBoxPadding->setChecked(false);
	ui.lineEditPaddingMin->setText("5");
	ui.lineEditPaddingMax->setText("5");
	ui.checkBoxDoors->setChecked(false);
	ui.lineEditDRelativeHeightMin->setText("0.2");
	ui.lineEditDRelativeHeightMax->setText("0.4");
	ui.lineEditDRelativeWidthMin->setText("0.7");
	ui.lineEditDRelativeWidthMax->setText("0.9");
	ui.lineEditNDMin->setText("2");
	ui.lineEditNDMax->setText("4");

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
	connect(ui.pushButtonInputPath, SIGNAL(clicked()), this, SLOT(onGetPath()));
	connect(ui.checkBoxWindowDis, SIGNAL(clicked()), this, SLOT(onUseWindowDis()));
	connect(ui.checkBoxWindowProb, SIGNAL(clicked()), this, SLOT(onUseWindowProb()));
	connect(ui.checkBoxPadding, SIGNAL(clicked()), this, SLOT(onUsePadding()));
	connect(ui.checkBoxDoors, SIGNAL(clicked()), this, SLOT(onUseDoors()));

	onUseWindowDis();
	onUseWindowProb();
	onUsePadding();
	onUseDoors();
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

bool GenerateFacadeOptionDialog::isAllowDoors(){
	return ui.checkBoxDoors->isChecked();
}

std::pair<double, double> GenerateFacadeOptionDialog::getDRelativeWidth(){
	return std::make_pair(ui.lineEditDRelativeWidthMin->text().toDouble(), ui.lineEditDRelativeWidthMax->text().toDouble());
}

std::pair<double, double> GenerateFacadeOptionDialog::getDRelativeHeight(){
	return std::make_pair(ui.lineEditDRelativeHeightMin->text().toDouble(), ui.lineEditDRelativeHeightMax->text().toDouble());
}

std::pair<int, int> GenerateFacadeOptionDialog::getND(){
	return std::make_pair(ui.lineEditNDMin->text().toInt(), ui.lineEditNDMax->text().toInt());
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

void GenerateFacadeOptionDialog::onUseDoors(){
	ui.groupBox_3->setEnabled(ui.checkBoxDoors->isChecked());
}