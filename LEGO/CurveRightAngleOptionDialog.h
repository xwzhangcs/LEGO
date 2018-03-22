#ifndef CURVERIGHTANGLEOPTIONDIALOG_H
#define CURVERIGHTANGLEOPTIONDIALOG_H

#include <QDialog>
#include "ui_CurveRightAngleOptionDialog.h"

class CurveRightAngleOptionDialog : public QDialog {
	Q_OBJECT

private:
	Ui::CurveRightAngleOptionDialog ui;

public:
	CurveRightAngleOptionDialog(QWidget *parent = 0);
	~CurveRightAngleOptionDialog();

	int getEpsilon();
	double getCurveThreshold();
	double getAngleThreshold();
	double getLayeringThreshold();
	double getSnapVertexThreshold();
	double getSnapEdgeThreshold();

public slots:
	void onOK();
	void onCancel();
};

#endif // CURVERIGHTANGLEOPTIONDIALOG_H
