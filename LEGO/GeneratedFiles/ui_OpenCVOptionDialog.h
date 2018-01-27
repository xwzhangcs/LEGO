/********************************************************************************
** Form generated from reading UI file 'OpenCVOptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPENCVOPTIONDIALOG_H
#define UI_OPENCVOPTIONDIALOG_H

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

class Ui_OpenCVOptionDialog
{
public:
    QLabel *label;
    QPushButton *pushButtonOK;
    QPushButton *pushButtonCancel;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBoxSlicingThreshold;
    QSpinBox *spinBoxEpsilon;

    void setupUi(QDialog *OpenCVOptionDialog)
    {
        if (OpenCVOptionDialog->objectName().isEmpty())
            OpenCVOptionDialog->setObjectName(QStringLiteral("OpenCVOptionDialog"));
        OpenCVOptionDialog->resize(203, 122);
        label = new QLabel(OpenCVOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 47, 21));
        pushButtonOK = new QPushButton(OpenCVOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(10, 80, 81, 31));
        pushButtonCancel = new QPushButton(OpenCVOptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(110, 80, 81, 31));
        label_2 = new QLabel(OpenCVOptionDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 40, 91, 21));
        doubleSpinBoxSlicingThreshold = new QDoubleSpinBox(OpenCVOptionDialog);
        doubleSpinBoxSlicingThreshold->setObjectName(QStringLiteral("doubleSpinBoxSlicingThreshold"));
        doubleSpinBoxSlicingThreshold->setGeometry(QRect(120, 40, 62, 22));
        spinBoxEpsilon = new QSpinBox(OpenCVOptionDialog);
        spinBoxEpsilon->setObjectName(QStringLiteral("spinBoxEpsilon"));
        spinBoxEpsilon->setGeometry(QRect(120, 10, 61, 22));

        retranslateUi(OpenCVOptionDialog);

        QMetaObject::connectSlotsByName(OpenCVOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *OpenCVOptionDialog)
    {
        OpenCVOptionDialog->setWindowTitle(QApplication::translate("OpenCVOptionDialog", "OpenCVOptionDialog", 0));
        label->setText(QApplication::translate("OpenCVOptionDialog", "Epsilon:", 0));
        pushButtonOK->setText(QApplication::translate("OpenCVOptionDialog", "OK", 0));
        pushButtonCancel->setText(QApplication::translate("OpenCVOptionDialog", "Cancel", 0));
        label_2->setText(QApplication::translate("OpenCVOptionDialog", "Slicing threshold:", 0));
    } // retranslateUi

};

namespace Ui {
    class OpenCVOptionDialog: public Ui_OpenCVOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENCVOPTIONDIALOG_H
