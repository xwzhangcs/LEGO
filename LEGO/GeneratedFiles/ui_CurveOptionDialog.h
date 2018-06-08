/********************************************************************************
** Form generated from reading UI file 'CurveOptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CURVEOPTIONDIALOG_H
#define UI_CURVEOPTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_CurveOptionDialog
{
public:
    QPushButton *pushButtonOK;
    QDoubleSpinBox *doubleSpinBoxLayeringThreshold;
    QLabel *label_3;
    QLabel *label_2;
    QSpinBox *spinBoxEpsilon;
    QPushButton *pushButtonCancel;
    QDoubleSpinBox *doubleSpinBoxSnappingThreshold;
    QLabel *label;
    QLabel *label_4;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBoxCurveThreshold;
    QLineEdit *lineEditOrientation;
    QLineEdit *lineEditMinContourArea;
    QLabel *label_6;
    QCheckBox *checkBoxAllowTriangleContour;
    QLabel *label_7;
    QLineEdit *lineEditMaxOBBRatio;

    void setupUi(QDialog *CurveOptionDialog)
    {
        if (CurveOptionDialog->objectName().isEmpty())
            CurveOptionDialog->setObjectName(QStringLiteral("CurveOptionDialog"));
        CurveOptionDialog->resize(242, 294);
        pushButtonOK = new QPushButton(CurveOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(30, 250, 81, 31));
        doubleSpinBoxLayeringThreshold = new QDoubleSpinBox(CurveOptionDialog);
        doubleSpinBoxLayeringThreshold->setObjectName(QStringLiteral("doubleSpinBoxLayeringThreshold"));
        doubleSpinBoxLayeringThreshold->setGeometry(QRect(160, 70, 62, 22));
        label_3 = new QLabel(CurveOptionDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 100, 131, 21));
        label_2 = new QLabel(CurveOptionDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 70, 91, 21));
        spinBoxEpsilon = new QSpinBox(CurveOptionDialog);
        spinBoxEpsilon->setObjectName(QStringLiteral("spinBoxEpsilon"));
        spinBoxEpsilon->setGeometry(QRect(160, 10, 61, 22));
        pushButtonCancel = new QPushButton(CurveOptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(140, 250, 81, 31));
        doubleSpinBoxSnappingThreshold = new QDoubleSpinBox(CurveOptionDialog);
        doubleSpinBoxSnappingThreshold->setObjectName(QStringLiteral("doubleSpinBoxSnappingThreshold"));
        doubleSpinBoxSnappingThreshold->setGeometry(QRect(160, 100, 62, 22));
        label = new QLabel(CurveOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 47, 21));
        label_4 = new QLabel(CurveOptionDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 130, 131, 21));
        label_5 = new QLabel(CurveOptionDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 40, 91, 21));
        doubleSpinBoxCurveThreshold = new QDoubleSpinBox(CurveOptionDialog);
        doubleSpinBoxCurveThreshold->setObjectName(QStringLiteral("doubleSpinBoxCurveThreshold"));
        doubleSpinBoxCurveThreshold->setGeometry(QRect(160, 40, 62, 22));
        lineEditOrientation = new QLineEdit(CurveOptionDialog);
        lineEditOrientation->setObjectName(QStringLiteral("lineEditOrientation"));
        lineEditOrientation->setGeometry(QRect(92, 130, 131, 20));
        lineEditOrientation->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditMinContourArea = new QLineEdit(CurveOptionDialog);
        lineEditMinContourArea->setObjectName(QStringLiteral("lineEditMinContourArea"));
        lineEditMinContourArea->setGeometry(QRect(162, 160, 61, 20));
        lineEditMinContourArea->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_6 = new QLabel(CurveOptionDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 160, 131, 21));
        checkBoxAllowTriangleContour = new QCheckBox(CurveOptionDialog);
        checkBoxAllowTriangleContour->setObjectName(QStringLiteral("checkBoxAllowTriangleContour"));
        checkBoxAllowTriangleContour->setGeometry(QRect(20, 190, 141, 17));
        label_7 = new QLabel(CurveOptionDialog);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(18, 210, 131, 21));
        lineEditMaxOBBRatio = new QLineEdit(CurveOptionDialog);
        lineEditMaxOBBRatio->setObjectName(QStringLiteral("lineEditMaxOBBRatio"));
        lineEditMaxOBBRatio->setGeometry(QRect(160, 210, 61, 20));
        lineEditMaxOBBRatio->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        retranslateUi(CurveOptionDialog);

        QMetaObject::connectSlotsByName(CurveOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *CurveOptionDialog)
    {
        CurveOptionDialog->setWindowTitle(QApplication::translate("CurveOptionDialog", "Option Dialog", Q_NULLPTR));
        pushButtonOK->setText(QApplication::translate("CurveOptionDialog", "OK", Q_NULLPTR));
        label_3->setText(QApplication::translate("CurveOptionDialog", "Snapping threshold:", Q_NULLPTR));
        label_2->setText(QApplication::translate("CurveOptionDialog", "Layering threshold:", Q_NULLPTR));
        pushButtonCancel->setText(QApplication::translate("CurveOptionDialog", "Cancel", Q_NULLPTR));
        label->setText(QApplication::translate("CurveOptionDialog", "Epsilon:", Q_NULLPTR));
        label_4->setText(QApplication::translate("CurveOptionDialog", "Orientation:", Q_NULLPTR));
        label_5->setText(QApplication::translate("CurveOptionDialog", "Curve threshold:", Q_NULLPTR));
        label_6->setText(QApplication::translate("CurveOptionDialog", "Minimum contour area:", Q_NULLPTR));
        checkBoxAllowTriangleContour->setText(QApplication::translate("CurveOptionDialog", "Allow triangle contour", Q_NULLPTR));
        label_7->setText(QApplication::translate("CurveOptionDialog", "Maximum OBB ratio:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CurveOptionDialog: public Ui_CurveOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CURVEOPTIONDIALOG_H
