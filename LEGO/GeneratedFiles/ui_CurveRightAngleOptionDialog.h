/********************************************************************************
** Form generated from reading UI file 'CurveRightAngleOptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CURVERIGHTANGLEOPTIONDIALOG_H
#define UI_CURVERIGHTANGLEOPTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_CurveRightAngleOptionDialog
{
public:
    QLabel *label_2;
    QLabel *label_4;
    QDoubleSpinBox *doubleSpinBoxLayeringThreshold;
    QDoubleSpinBox *doubleSpinBoxSnapVertexThreshold;
    QSpinBox *spinBoxEpsilon;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBoxCurveThreshold;
    QPushButton *pushButtonCancel;
    QLabel *label;
    QPushButton *pushButtonOK;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBoxSnapEdgeThreshold;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBoxAngleThreshold;

    void setupUi(QDialog *CurveRightAngleOptionDialog)
    {
        if (CurveRightAngleOptionDialog->objectName().isEmpty())
            CurveRightAngleOptionDialog->setObjectName(QStringLiteral("CurveRightAngleOptionDialog"));
        CurveRightAngleOptionDialog->resize(242, 238);
        label_2 = new QLabel(CurveRightAngleOptionDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 100, 91, 21));
        label_4 = new QLabel(CurveRightAngleOptionDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 160, 131, 21));
        doubleSpinBoxLayeringThreshold = new QDoubleSpinBox(CurveRightAngleOptionDialog);
        doubleSpinBoxLayeringThreshold->setObjectName(QStringLiteral("doubleSpinBoxLayeringThreshold"));
        doubleSpinBoxLayeringThreshold->setGeometry(QRect(160, 100, 62, 22));
        doubleSpinBoxSnapVertexThreshold = new QDoubleSpinBox(CurveRightAngleOptionDialog);
        doubleSpinBoxSnapVertexThreshold->setObjectName(QStringLiteral("doubleSpinBoxSnapVertexThreshold"));
        doubleSpinBoxSnapVertexThreshold->setGeometry(QRect(160, 130, 62, 22));
        spinBoxEpsilon = new QSpinBox(CurveRightAngleOptionDialog);
        spinBoxEpsilon->setObjectName(QStringLiteral("spinBoxEpsilon"));
        spinBoxEpsilon->setGeometry(QRect(160, 10, 61, 22));
        label_5 = new QLabel(CurveRightAngleOptionDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 40, 91, 21));
        doubleSpinBoxCurveThreshold = new QDoubleSpinBox(CurveRightAngleOptionDialog);
        doubleSpinBoxCurveThreshold->setObjectName(QStringLiteral("doubleSpinBoxCurveThreshold"));
        doubleSpinBoxCurveThreshold->setGeometry(QRect(160, 40, 62, 22));
        pushButtonCancel = new QPushButton(CurveRightAngleOptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(140, 190, 81, 31));
        label = new QLabel(CurveRightAngleOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 47, 21));
        pushButtonOK = new QPushButton(CurveRightAngleOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(30, 190, 81, 31));
        label_3 = new QLabel(CurveRightAngleOptionDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 130, 131, 21));
        doubleSpinBoxSnapEdgeThreshold = new QDoubleSpinBox(CurveRightAngleOptionDialog);
        doubleSpinBoxSnapEdgeThreshold->setObjectName(QStringLiteral("doubleSpinBoxSnapEdgeThreshold"));
        doubleSpinBoxSnapEdgeThreshold->setGeometry(QRect(160, 160, 62, 22));
        label_6 = new QLabel(CurveRightAngleOptionDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 70, 91, 21));
        doubleSpinBoxAngleThreshold = new QDoubleSpinBox(CurveRightAngleOptionDialog);
        doubleSpinBoxAngleThreshold->setObjectName(QStringLiteral("doubleSpinBoxAngleThreshold"));
        doubleSpinBoxAngleThreshold->setGeometry(QRect(160, 70, 62, 22));

        retranslateUi(CurveRightAngleOptionDialog);

        QMetaObject::connectSlotsByName(CurveRightAngleOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *CurveRightAngleOptionDialog)
    {
        CurveRightAngleOptionDialog->setWindowTitle(QApplication::translate("CurveRightAngleOptionDialog", "CurveRightAngleOptionDialog", 0));
        label_2->setText(QApplication::translate("CurveRightAngleOptionDialog", "Layering threshold:", 0));
        label_4->setText(QApplication::translate("CurveRightAngleOptionDialog", "Snap-to-edge threshold:", 0));
        label_5->setText(QApplication::translate("CurveRightAngleOptionDialog", "Curve threshold:", 0));
        pushButtonCancel->setText(QApplication::translate("CurveRightAngleOptionDialog", "Cancel", 0));
        label->setText(QApplication::translate("CurveRightAngleOptionDialog", "Epsilon:", 0));
        pushButtonOK->setText(QApplication::translate("CurveRightAngleOptionDialog", "OK", 0));
        label_3->setText(QApplication::translate("CurveRightAngleOptionDialog", "Snap-to-vertex threshold:", 0));
        label_6->setText(QApplication::translate("CurveRightAngleOptionDialog", "Angle threshold:", 0));
    } // retranslateUi

};

namespace Ui {
    class CurveRightAngleOptionDialog: public Ui_CurveRightAngleOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CURVERIGHTANGLEOPTIONDIALOG_H
