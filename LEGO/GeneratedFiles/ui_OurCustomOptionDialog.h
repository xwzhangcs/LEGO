/********************************************************************************
** Form generated from reading UI file 'OurCustomOptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OURCUSTOMOPTIONDIALOG_H
#define UI_OURCUSTOMOPTIONDIALOG_H

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

class Ui_OurCustomOptionDialog
{
public:
    QLabel *label;
    QSpinBox *spinBoxResolution;
    QPushButton *pushButtonCancel;
    QPushButton *pushButtonOK;
    QDoubleSpinBox *doubleSpinBoxLayeringThreshold;
    QLabel *label_2;

    void setupUi(QDialog *OurCustomOptionDialog)
    {
        if (OurCustomOptionDialog->objectName().isEmpty())
            OurCustomOptionDialog->setObjectName(QStringLiteral("OurCustomOptionDialog"));
        OurCustomOptionDialog->resize(202, 121);
        label = new QLabel(OurCustomOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 61, 21));
        spinBoxResolution = new QSpinBox(OurCustomOptionDialog);
        spinBoxResolution->setObjectName(QStringLiteral("spinBoxResolution"));
        spinBoxResolution->setGeometry(QRect(120, 10, 61, 22));
        pushButtonCancel = new QPushButton(OurCustomOptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(110, 80, 81, 31));
        pushButtonOK = new QPushButton(OurCustomOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(10, 80, 81, 31));
        doubleSpinBoxLayeringThreshold = new QDoubleSpinBox(OurCustomOptionDialog);
        doubleSpinBoxLayeringThreshold->setObjectName(QStringLiteral("doubleSpinBoxLayeringThreshold"));
        doubleSpinBoxLayeringThreshold->setGeometry(QRect(120, 40, 62, 22));
        label_2 = new QLabel(OurCustomOptionDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 40, 91, 21));

        retranslateUi(OurCustomOptionDialog);

        QMetaObject::connectSlotsByName(OurCustomOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *OurCustomOptionDialog)
    {
        OurCustomOptionDialog->setWindowTitle(QApplication::translate("OurCustomOptionDialog", "OurCustomOptionDialog", 0));
        label->setText(QApplication::translate("OurCustomOptionDialog", "Resolution:", 0));
        pushButtonCancel->setText(QApplication::translate("OurCustomOptionDialog", "Cancel", 0));
        pushButtonOK->setText(QApplication::translate("OurCustomOptionDialog", "OK", 0));
        label_2->setText(QApplication::translate("OurCustomOptionDialog", "Layering threshold:", 0));
    } // retranslateUi

};

namespace Ui {
    class OurCustomOptionDialog: public Ui_OurCustomOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OURCUSTOMOPTIONDIALOG_H
