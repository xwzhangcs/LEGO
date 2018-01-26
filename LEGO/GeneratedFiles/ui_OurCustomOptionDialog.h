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

    void setupUi(QDialog *OurCustomOptionDialog)
    {
        if (OurCustomOptionDialog->objectName().isEmpty())
            OurCustomOptionDialog->setObjectName(QStringLiteral("OurCustomOptionDialog"));
        OurCustomOptionDialog->resize(189, 92);
        label = new QLabel(OurCustomOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 10, 61, 21));
        spinBoxResolution = new QSpinBox(OurCustomOptionDialog);
        spinBoxResolution->setObjectName(QStringLiteral("spinBoxResolution"));
        spinBoxResolution->setGeometry(QRect(100, 10, 61, 22));
        pushButtonCancel = new QPushButton(OurCustomOptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(100, 50, 81, 31));
        pushButtonOK = new QPushButton(OurCustomOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(10, 50, 81, 31));

        retranslateUi(OurCustomOptionDialog);

        QMetaObject::connectSlotsByName(OurCustomOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *OurCustomOptionDialog)
    {
        OurCustomOptionDialog->setWindowTitle(QApplication::translate("OurCustomOptionDialog", "OurCustomOptionDialog", 0));
        label->setText(QApplication::translate("OurCustomOptionDialog", "Resolution:", 0));
        pushButtonCancel->setText(QApplication::translate("OurCustomOptionDialog", "Cancel", 0));
        pushButtonOK->setText(QApplication::translate("OurCustomOptionDialog", "OK", 0));
    } // retranslateUi

};

namespace Ui {
    class OurCustomOptionDialog: public Ui_OurCustomOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OURCUSTOMOPTIONDIALOG_H
