#ifndef OURCUSTOMOPTIONDIALOG_H
#define OURCUSTOMOPTIONDIALOG_H

#include <QDialog>
#include "ui_OurCustomOptionDialog.h"

class OurCustomOptionDialog : public QDialog {
	Q_OBJECT

private:
	Ui::OurCustomOptionDialog ui;

public:
	OurCustomOptionDialog(QWidget *parent = 0);
	~OurCustomOptionDialog();
	
	int getResolution();
	double getLayeringThreshold();

public slots:
	void onOK();
	void onCancel();
};

#endif // OURCUSTOMOPTIONDIALOG_H
