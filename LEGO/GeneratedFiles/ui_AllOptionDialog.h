/********************************************************************************
** Form generated from reading UI file 'AllOptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALLOPTIONDIALOG_H
#define UI_ALLOPTIONDIALOG_H

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

class Ui_AllOptionDialog
{
public:
    QPushButton *pushButtonOK;
    QDoubleSpinBox *doubleSpinBoxAlpha;
    QLabel *label;
    QPushButton *pushButtonCancel;

    void setupUi(QDialog *AllOptionDialog)
    {
        if (AllOptionDialog->objectName().isEmpty())
            AllOptionDialog->setObjectName(QStringLiteral("AllOptionDialog"));
        AllOptionDialog->resize(191, 82);
        pushButtonOK = new QPushButton(AllOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(10, 40, 81, 31));
        doubleSpinBoxAlpha = new QDoubleSpinBox(AllOptionDialog);
        doubleSpinBoxAlpha->setObjectName(QStringLiteral("doubleSpinBoxAlpha"));
        doubleSpinBoxAlpha->setGeometry(QRect(110, 10, 62, 22));
        label = new QLabel(AllOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 47, 21));
        pushButtonCancel = new QPushButton(AllOptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(100, 40, 81, 31));

        retranslateUi(AllOptionDialog);

        QMetaObject::connectSlotsByName(AllOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *AllOptionDialog)
    {
        AllOptionDialog->setWindowTitle(QApplication::translate("AllOptionDialog", "AllOptionDialog", Q_NULLPTR));
        pushButtonOK->setText(QApplication::translate("AllOptionDialog", "OK", Q_NULLPTR));
        label->setText(QApplication::translate("AllOptionDialog", "Alpha:", Q_NULLPTR));
        pushButtonCancel->setText(QApplication::translate("AllOptionDialog", "Cancel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AllOptionDialog: public Ui_AllOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALLOPTIONDIALOG_H
