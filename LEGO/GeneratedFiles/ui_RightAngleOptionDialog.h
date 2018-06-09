/********************************************************************************
** Form generated from reading UI file 'RightAngleOptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RIGHTANGLEOPTIONDIALOG_H
#define UI_RIGHTANGLEOPTIONDIALOG_H

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

class Ui_RightAngleOptionDialog
{
public:
    QLabel *label;
    QSpinBox *spinBoxResolution;
    QPushButton *pushButtonCancel;
    QPushButton *pushButtonOK;
    QDoubleSpinBox *doubleSpinBoxLayeringThreshold;
    QLabel *label_2;
    QLabel *label_4;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBoxSnappingThreshold;
    QLineEdit *lineEditOrientation;
    QLineEdit *lineEditMinContourArea;
    QLabel *label_5;
    QCheckBox *checkBoxAllowTriangleContour;
    QLineEdit *lineEditMaxOBBRatio;
    QLabel *label_6;
    QCheckBox *checkBoxAllowOverhang;

    void setupUi(QDialog *RightAngleOptionDialog)
    {
        if (RightAngleOptionDialog->objectName().isEmpty())
            RightAngleOptionDialog->setObjectName(QStringLiteral("RightAngleOptionDialog"));
        RightAngleOptionDialog->resize(246, 281);
        RightAngleOptionDialog->setMinimumSize(QSize(246, 172));
        RightAngleOptionDialog->setMaximumSize(QSize(346, 300));
        label = new QLabel(RightAngleOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 61, 21));
        spinBoxResolution = new QSpinBox(RightAngleOptionDialog);
        spinBoxResolution->setObjectName(QStringLiteral("spinBoxResolution"));
        spinBoxResolution->setGeometry(QRect(160, 10, 61, 22));
        pushButtonCancel = new QPushButton(RightAngleOptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(130, 240, 81, 31));
        pushButtonOK = new QPushButton(RightAngleOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(30, 240, 81, 31));
        doubleSpinBoxLayeringThreshold = new QDoubleSpinBox(RightAngleOptionDialog);
        doubleSpinBoxLayeringThreshold->setObjectName(QStringLiteral("doubleSpinBoxLayeringThreshold"));
        doubleSpinBoxLayeringThreshold->setGeometry(QRect(160, 40, 62, 22));
        label_2 = new QLabel(RightAngleOptionDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 40, 91, 21));
        label_4 = new QLabel(RightAngleOptionDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 100, 131, 21));
        label_3 = new QLabel(RightAngleOptionDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 70, 131, 21));
        doubleSpinBoxSnappingThreshold = new QDoubleSpinBox(RightAngleOptionDialog);
        doubleSpinBoxSnappingThreshold->setObjectName(QStringLiteral("doubleSpinBoxSnappingThreshold"));
        doubleSpinBoxSnappingThreshold->setGeometry(QRect(160, 70, 62, 22));
        lineEditOrientation = new QLineEdit(RightAngleOptionDialog);
        lineEditOrientation->setObjectName(QStringLiteral("lineEditOrientation"));
        lineEditOrientation->setGeometry(QRect(92, 100, 131, 20));
        lineEditOrientation->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditMinContourArea = new QLineEdit(RightAngleOptionDialog);
        lineEditMinContourArea->setObjectName(QStringLiteral("lineEditMinContourArea"));
        lineEditMinContourArea->setGeometry(QRect(162, 130, 61, 20));
        lineEditMinContourArea->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_5 = new QLabel(RightAngleOptionDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 130, 131, 21));
        checkBoxAllowTriangleContour = new QCheckBox(RightAngleOptionDialog);
        checkBoxAllowTriangleContour->setObjectName(QStringLiteral("checkBoxAllowTriangleContour"));
        checkBoxAllowTriangleContour->setGeometry(QRect(20, 190, 141, 17));
        lineEditMaxOBBRatio = new QLineEdit(RightAngleOptionDialog);
        lineEditMaxOBBRatio->setObjectName(QStringLiteral("lineEditMaxOBBRatio"));
        lineEditMaxOBBRatio->setGeometry(QRect(160, 160, 61, 20));
        lineEditMaxOBBRatio->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_6 = new QLabel(RightAngleOptionDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 160, 131, 21));
        checkBoxAllowOverhang = new QCheckBox(RightAngleOptionDialog);
        checkBoxAllowOverhang->setObjectName(QStringLiteral("checkBoxAllowOverhang"));
        checkBoxAllowOverhang->setGeometry(QRect(20, 210, 141, 17));

        retranslateUi(RightAngleOptionDialog);

        QMetaObject::connectSlotsByName(RightAngleOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *RightAngleOptionDialog)
    {
        RightAngleOptionDialog->setWindowTitle(QApplication::translate("RightAngleOptionDialog", "Option Dialog", Q_NULLPTR));
        label->setText(QApplication::translate("RightAngleOptionDialog", "Resolution:", Q_NULLPTR));
        pushButtonCancel->setText(QApplication::translate("RightAngleOptionDialog", "Cancel", Q_NULLPTR));
        pushButtonOK->setText(QApplication::translate("RightAngleOptionDialog", "OK", Q_NULLPTR));
        label_2->setText(QApplication::translate("RightAngleOptionDialog", "Layering threshold:", Q_NULLPTR));
        label_4->setText(QApplication::translate("RightAngleOptionDialog", "Orientation:", Q_NULLPTR));
        label_3->setText(QApplication::translate("RightAngleOptionDialog", "Snapping threshold:", Q_NULLPTR));
        label_5->setText(QApplication::translate("RightAngleOptionDialog", "Minimum contour area:", Q_NULLPTR));
        checkBoxAllowTriangleContour->setText(QApplication::translate("RightAngleOptionDialog", "Allow triangle contour", Q_NULLPTR));
        label_6->setText(QApplication::translate("RightAngleOptionDialog", "Maximum OBB ratio:", Q_NULLPTR));
        checkBoxAllowOverhang->setText(QApplication::translate("RightAngleOptionDialog", "Allow overhang", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class RightAngleOptionDialog: public Ui_RightAngleOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RIGHTANGLEOPTIONDIALOG_H
