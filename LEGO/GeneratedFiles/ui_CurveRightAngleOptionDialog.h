/********************************************************************************
** Form generated from reading UI file 'CurveRightAngleOptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CURVERIGHTANGLEOPTIONDIALOG_H
#define UI_CURVERIGHTANGLEOPTIONDIALOG_H

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

class Ui_CurveRightAngleOptionDialog
{
public:
    QLabel *label_2;
    QLabel *label_4;
    QDoubleSpinBox *doubleSpinBoxLayeringThreshold;
    QDoubleSpinBox *doubleSpinBoxSnappingThreshold;
    QSpinBox *spinBoxEpsilon;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBoxCurveThreshold;
    QPushButton *pushButtonCancel;
    QLabel *label;
    QPushButton *pushButtonOK;
    QLabel *label_3;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBoxAngleThreshold;
    QLineEdit *lineEditOrientation;
    QLabel *label_7;
    QLineEdit *lineEditMinContourArea;
    QCheckBox *checkBoxAllowTriangleContour;
    QLabel *label_8;
    QLineEdit *lineEditMaxOBBRatio;

    void setupUi(QDialog *CurveRightAngleOptionDialog)
    {
        if (CurveRightAngleOptionDialog->objectName().isEmpty())
            CurveRightAngleOptionDialog->setObjectName(QStringLiteral("CurveRightAngleOptionDialog"));
        CurveRightAngleOptionDialog->resize(242, 321);
        label_2 = new QLabel(CurveRightAngleOptionDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 100, 91, 21));
        label_4 = new QLabel(CurveRightAngleOptionDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 160, 131, 21));
        doubleSpinBoxLayeringThreshold = new QDoubleSpinBox(CurveRightAngleOptionDialog);
        doubleSpinBoxLayeringThreshold->setObjectName(QStringLiteral("doubleSpinBoxLayeringThreshold"));
        doubleSpinBoxLayeringThreshold->setGeometry(QRect(160, 100, 62, 22));
        doubleSpinBoxSnappingThreshold = new QDoubleSpinBox(CurveRightAngleOptionDialog);
        doubleSpinBoxSnappingThreshold->setObjectName(QStringLiteral("doubleSpinBoxSnappingThreshold"));
        doubleSpinBoxSnappingThreshold->setGeometry(QRect(160, 130, 62, 22));
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
        pushButtonCancel->setGeometry(QRect(140, 280, 81, 31));
        label = new QLabel(CurveRightAngleOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 47, 21));
        pushButtonOK = new QPushButton(CurveRightAngleOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(30, 280, 81, 31));
        label_3 = new QLabel(CurveRightAngleOptionDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 130, 131, 21));
        label_6 = new QLabel(CurveRightAngleOptionDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 70, 91, 21));
        doubleSpinBoxAngleThreshold = new QDoubleSpinBox(CurveRightAngleOptionDialog);
        doubleSpinBoxAngleThreshold->setObjectName(QStringLiteral("doubleSpinBoxAngleThreshold"));
        doubleSpinBoxAngleThreshold->setGeometry(QRect(160, 70, 62, 22));
        lineEditOrientation = new QLineEdit(CurveRightAngleOptionDialog);
        lineEditOrientation->setObjectName(QStringLiteral("lineEditOrientation"));
        lineEditOrientation->setGeometry(QRect(92, 160, 131, 20));
        lineEditOrientation->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_7 = new QLabel(CurveRightAngleOptionDialog);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(18, 190, 131, 21));
        lineEditMinContourArea = new QLineEdit(CurveRightAngleOptionDialog);
        lineEditMinContourArea->setObjectName(QStringLiteral("lineEditMinContourArea"));
        lineEditMinContourArea->setGeometry(QRect(160, 190, 61, 20));
        lineEditMinContourArea->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        checkBoxAllowTriangleContour = new QCheckBox(CurveRightAngleOptionDialog);
        checkBoxAllowTriangleContour->setObjectName(QStringLiteral("checkBoxAllowTriangleContour"));
        checkBoxAllowTriangleContour->setGeometry(QRect(20, 220, 131, 17));
        label_8 = new QLabel(CurveRightAngleOptionDialog);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(18, 240, 131, 21));
        lineEditMaxOBBRatio = new QLineEdit(CurveRightAngleOptionDialog);
        lineEditMaxOBBRatio->setObjectName(QStringLiteral("lineEditMaxOBBRatio"));
        lineEditMaxOBBRatio->setGeometry(QRect(160, 240, 61, 20));
        lineEditMaxOBBRatio->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        retranslateUi(CurveRightAngleOptionDialog);

        QMetaObject::connectSlotsByName(CurveRightAngleOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *CurveRightAngleOptionDialog)
    {
        CurveRightAngleOptionDialog->setWindowTitle(QApplication::translate("CurveRightAngleOptionDialog", "CurveRightAngleOptionDialog", Q_NULLPTR));
        label_2->setText(QApplication::translate("CurveRightAngleOptionDialog", "Layering threshold:", Q_NULLPTR));
        label_4->setText(QApplication::translate("CurveRightAngleOptionDialog", "Orientation:", Q_NULLPTR));
        label_5->setText(QApplication::translate("CurveRightAngleOptionDialog", "Curve threshold:", Q_NULLPTR));
        pushButtonCancel->setText(QApplication::translate("CurveRightAngleOptionDialog", "Cancel", Q_NULLPTR));
        label->setText(QApplication::translate("CurveRightAngleOptionDialog", "Epsilon:", Q_NULLPTR));
        pushButtonOK->setText(QApplication::translate("CurveRightAngleOptionDialog", "OK", Q_NULLPTR));
        label_3->setText(QApplication::translate("CurveRightAngleOptionDialog", "Snapping threshold:", Q_NULLPTR));
        label_6->setText(QApplication::translate("CurveRightAngleOptionDialog", "Angle threshold:", Q_NULLPTR));
        label_7->setText(QApplication::translate("CurveRightAngleOptionDialog", "Minimum contour area:", Q_NULLPTR));
        checkBoxAllowTriangleContour->setText(QApplication::translate("CurveRightAngleOptionDialog", "Allow triangle contour", Q_NULLPTR));
        label_8->setText(QApplication::translate("CurveRightAngleOptionDialog", "Maximum OBB ratio:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CurveRightAngleOptionDialog: public Ui_CurveRightAngleOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CURVERIGHTANGLEOPTIONDIALOG_H
