#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include "ui_OptionDialog.h"

class OptionDialog : public QDialog {
	Q_OBJECT

private:
	Ui::OptionDialog ui;

public:
	OptionDialog(QWidget *parent = 0);
	~OptionDialog();

	double getAlpha();

public slots:
	void onOK();
	void onCancel();
};

#endif // OPTIONDIALOG_H
