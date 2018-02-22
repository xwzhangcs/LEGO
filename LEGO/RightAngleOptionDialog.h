#ifndef RIGHTANGLEOPTIONDIALOG_H
#define RIGHTANGLEOPTIONDIALOG_H

#include <QDialog>
#include "ui_RightAngleOptionDialog.h"

class RightAngleOptionDialog : public QDialog {
	Q_OBJECT

private:
	Ui::RightAngleOptionDialog ui;

public:
	RightAngleOptionDialog(QWidget *parent = 0);
	~RightAngleOptionDialog();
	
	int getResolution();
	double getLayeringThreshold();
	double getSnapVertexThreshold();
	double getSnapEdgeThreshold();

public slots:
	void onOK();
	void onCancel();
};

#endif // RIGHTANGLEOPTIONDIALOG_H
