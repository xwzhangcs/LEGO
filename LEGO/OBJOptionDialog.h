#ifndef OBJOPTIONDIALOG_H
#define OBJOPTIONDIALOG_H

#include <QDialog>
#include "ui_OBJOptionDialog.h"

class OBJOptionDialog : public QDialog {
	Q_OBJECT

private:
	Ui::OBJOptionDialog ui;

public:
	OBJOptionDialog(QWidget *parent = 0);
	~OBJOptionDialog();

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

#endif // OBJOPTIONDIALOG_H
