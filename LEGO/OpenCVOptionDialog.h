#ifndef OPENCVOPTIONDIALOG_H
#define OPENCVOPTIONDIALOG_H

#include <QDialog>
#include "ui_OpenCVOptionDialog.h"

class OpenCVOptionDialog : public QDialog {
	Q_OBJECT

private:
	Ui::OpenCVOptionDialog ui;

public:
	OpenCVOptionDialog(QWidget *parent = 0);
	~OpenCVOptionDialog();

	double getEpsilon();
	
public slots:
	void onOK();
	void onCancel();
};

#endif // OPENCVOPTIONDIALOG_H
