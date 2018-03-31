#ifndef PLYOPTIONDIALOG_H
#define PLYOPTIONDIALOG_H

#include <QDialog>
#include "ui_PLYOptionDialog.h"

class PLYOptionDialog : public QDialog {
	Q_OBJECT

private:
	Ui::PLYOptionDialog ui;

public:
	PLYOptionDialog(QWidget *parent = 0);
	~PLYOptionDialog();

	double getOffsetX();
	double getOffsetY();
	double getOffsetZ();
	double getScale();
	QString getFileName();

public slots:
	void onFileName();
	void onOK();
	void onCancel();
};

#endif // PLYOPTIONDIALOG_H
