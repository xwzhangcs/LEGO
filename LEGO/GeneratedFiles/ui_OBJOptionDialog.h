/********************************************************************************
** Form generated from reading UI file 'OBJOptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OBJOPTIONDIALOG_H
#define UI_OBJOPTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_OBJOptionDialog
{
public:
    QLineEdit *lineEditFileName;
    QPushButton *pushButtonCancel;
    QLineEdit *lineEditOffsetX;
    QLabel *label_4;
    QPushButton *pushButtonD4;
    QLineEdit *lineEditOffsetZ;
    QLineEdit *lineEditOffsetY;
    QLabel *label_5;
    QLabel *label_2;
    QLabel *label;
    QPushButton *pushButtonFileName;
    QPushButton *pushButtonD1;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBoxScale;
    QPushButton *pushButtonOK;
    QPushButton *pushButtonD3;
    QPushButton *pushButtonD2;

    void setupUi(QDialog *OBJOptionDialog)
    {
        if (OBJOptionDialog->objectName().isEmpty())
            OBJOptionDialog->setObjectName(QStringLiteral("OBJOptionDialog"));
        OBJOptionDialog->resize(422, 200);
        lineEditFileName = new QLineEdit(OBJOptionDialog);
        lineEditFileName->setObjectName(QStringLiteral("lineEditFileName"));
        lineEditFileName->setGeometry(QRect(80, 130, 291, 20));
        pushButtonCancel = new QPushButton(OBJOptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(330, 160, 81, 31));
        lineEditOffsetX = new QLineEdit(OBJOptionDialog);
        lineEditOffsetX->setObjectName(QStringLiteral("lineEditOffsetX"));
        lineEditOffsetX->setGeometry(QRect(80, 10, 131, 20));
        label_4 = new QLabel(OBJOptionDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 100, 61, 21));
        pushButtonD4 = new QPushButton(OBJOptionDialog);
        pushButtonD4->setObjectName(QStringLiteral("pushButtonD4"));
        pushButtonD4->setGeometry(QRect(330, 50, 71, 31));
        lineEditOffsetZ = new QLineEdit(OBJOptionDialog);
        lineEditOffsetZ->setObjectName(QStringLiteral("lineEditOffsetZ"));
        lineEditOffsetZ->setGeometry(QRect(80, 70, 131, 20));
        lineEditOffsetY = new QLineEdit(OBJOptionDialog);
        lineEditOffsetY->setObjectName(QStringLiteral("lineEditOffsetY"));
        lineEditOffsetY->setGeometry(QRect(80, 40, 131, 20));
        label_5 = new QLabel(OBJOptionDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 130, 61, 21));
        label_2 = new QLabel(OBJOptionDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 40, 91, 21));
        label = new QLabel(OBJOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 10, 47, 21));
        pushButtonFileName = new QPushButton(OBJOptionDialog);
        pushButtonFileName->setObjectName(QStringLiteral("pushButtonFileName"));
        pushButtonFileName->setGeometry(QRect(380, 130, 31, 21));
        pushButtonD1 = new QPushButton(OBJOptionDialog);
        pushButtonD1->setObjectName(QStringLiteral("pushButtonD1"));
        pushButtonD1->setGeometry(QRect(240, 10, 71, 31));
        label_3 = new QLabel(OBJOptionDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 70, 71, 21));
        doubleSpinBoxScale = new QDoubleSpinBox(OBJOptionDialog);
        doubleSpinBoxScale->setObjectName(QStringLiteral("doubleSpinBoxScale"));
        doubleSpinBoxScale->setGeometry(QRect(80, 100, 71, 22));
        pushButtonOK = new QPushButton(OBJOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(230, 160, 81, 31));
        pushButtonD3 = new QPushButton(OBJOptionDialog);
        pushButtonD3->setObjectName(QStringLiteral("pushButtonD3"));
        pushButtonD3->setGeometry(QRect(240, 50, 71, 31));
        pushButtonD2 = new QPushButton(OBJOptionDialog);
        pushButtonD2->setObjectName(QStringLiteral("pushButtonD2"));
        pushButtonD2->setGeometry(QRect(330, 10, 71, 31));

        retranslateUi(OBJOptionDialog);

        QMetaObject::connectSlotsByName(OBJOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *OBJOptionDialog)
    {
        OBJOptionDialog->setWindowTitle(QApplication::translate("OBJOptionDialog", "OBJOptionDialog", 0));
        pushButtonCancel->setText(QApplication::translate("OBJOptionDialog", "Cancel", 0));
        label_4->setText(QApplication::translate("OBJOptionDialog", "Scale:", 0));
        pushButtonD4->setText(QApplication::translate("OBJOptionDialog", "D4", 0));
        label_5->setText(QApplication::translate("OBJOptionDialog", "File name:", 0));
        label_2->setText(QApplication::translate("OBJOptionDialog", "Offset Y:", 0));
        label->setText(QApplication::translate("OBJOptionDialog", "Offset X:", 0));
        pushButtonFileName->setText(QApplication::translate("OBJOptionDialog", "...", 0));
        pushButtonD1->setText(QApplication::translate("OBJOptionDialog", "D1", 0));
        label_3->setText(QApplication::translate("OBJOptionDialog", "Offset Z:", 0));
        pushButtonOK->setText(QApplication::translate("OBJOptionDialog", "OK", 0));
        pushButtonD3->setText(QApplication::translate("OBJOptionDialog", "D3", 0));
        pushButtonD2->setText(QApplication::translate("OBJOptionDialog", "D2", 0));
    } // retranslateUi

};

namespace Ui {
    class OBJOptionDialog: public Ui_OBJOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OBJOPTIONDIALOG_H
