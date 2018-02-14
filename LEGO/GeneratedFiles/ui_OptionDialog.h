/********************************************************************************
** Form generated from reading UI file 'OptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPTIONDIALOG_H
#define UI_OPTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_OptionDialog
{
public:
    QPushButton *pushButtonCancel;
    QPushButton *pushButtonOK;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBoxAlpha;

    void setupUi(QDialog *OptionDialog)
    {
        if (OptionDialog->objectName().isEmpty())
            OptionDialog->setObjectName(QStringLiteral("OptionDialog"));
        OptionDialog->resize(191, 83);
        pushButtonCancel = new QPushButton(OptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(100, 40, 81, 31));
        pushButtonOK = new QPushButton(OptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(10, 40, 81, 31));
        label = new QLabel(OptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 47, 21));
        doubleSpinBoxAlpha = new QDoubleSpinBox(OptionDialog);
        doubleSpinBoxAlpha->setObjectName(QStringLiteral("doubleSpinBoxAlpha"));
        doubleSpinBoxAlpha->setGeometry(QRect(110, 10, 62, 22));

        retranslateUi(OptionDialog);

        QMetaObject::connectSlotsByName(OptionDialog);
    } // setupUi

    void retranslateUi(QDialog *OptionDialog)
    {
        OptionDialog->setWindowTitle(QApplication::translate("OptionDialog", "OptionDialog", 0));
        pushButtonCancel->setText(QApplication::translate("OptionDialog", "Cancel", 0));
        pushButtonOK->setText(QApplication::translate("OptionDialog", "OK", 0));
        label->setText(QApplication::translate("OptionDialog", "Alpha:", 0));
    } // retranslateUi

};

namespace Ui {
    class OptionDialog: public Ui_OptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPTIONDIALOG_H
