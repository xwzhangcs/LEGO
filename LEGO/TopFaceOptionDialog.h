#ifndef TOPFACEOPTIONDIALOG_H
#define TOPFACEOPTIONDIALOG_H

#include <QDialog>
#include "ui_TopFaceOptionDialog.h"

class TopFaceOptionDialog : public QDialog {
	Q_OBJECT

private:
	Ui::TopFaceOptionDialog ui;

public:
	TopFaceOptionDialog(QWidget *parent = 0);
	~TopFaceOptionDialog();

	double getOffsetX();
	double getOffsetY();
	double getOffsetZ();
	double getScale();
	QString getFileName();

public slots:
	void onD1();
	void onD2();
	void onD3();
	void onD4();
	void onFileName();
	void onOK();
	void onCancel();
};

#endif // TOPFACEOPTIONDIALOG_H
