/********************************************************************************
** Form generated from reading UI file 'PLYOptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLYOPTIONDIALOG_H
#define UI_PLYOPTIONDIALOG_H

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

class Ui_PLYOptionDialog
{
public:
    QLabel *label_4;
    QPushButton *pushButtonCancel;
    QDoubleSpinBox *doubleSpinBoxScale;
    QPushButton *pushButtonOK;
    QLabel *label_2;
    QLabel *label;
    QLabel *label_3;
    QLineEdit *lineEditOffsetX;
    QLineEdit *lineEditOffsetY;
    QLineEdit *lineEditOffsetZ;
    QLabel *label_5;
    QLineEdit *lineEditFileName;
    QPushButton *pushButtonFileName;
    QPushButton *pushButtonD1;
    QPushButton *pushButtonD2;
    QPushButton *pushButtonD4;
    QPushButton *pushButtonD3;

    void setupUi(QDialog *PLYOptionDialog)
    {
        if (PLYOptionDialog->objectName().isEmpty())
            PLYOptionDialog->setObjectName(QStringLiteral("PLYOptionDialog"));
        PLYOptionDialog->resize(434, 199);
        label_4 = new QLabel(PLYOptionDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 100, 61, 21));
        pushButtonCancel = new QPushButton(PLYOptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(340, 160, 81, 31));
        doubleSpinBoxScale = new QDoubleSpinBox(PLYOptionDialog);
        doubleSpinBoxScale->setObjectName(QStringLiteral("doubleSpinBoxScale"));
        doubleSpinBoxScale->setGeometry(QRect(90, 100, 71, 22));
        pushButtonOK = new QPushButton(PLYOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(230, 160, 81, 31));
        label_2 = new QLabel(PLYOptionDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 40, 91, 21));
        label = new QLabel(PLYOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 47, 21));
        label_3 = new QLabel(PLYOptionDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 70, 71, 21));
        lineEditOffsetX = new QLineEdit(PLYOptionDialog);
        lineEditOffsetX->setObjectName(QStringLiteral("lineEditOffsetX"));
        lineEditOffsetX->setGeometry(QRect(90, 10, 131, 20));
        lineEditOffsetY = new QLineEdit(PLYOptionDialog);
        lineEditOffsetY->setObjectName(QStringLiteral("lineEditOffsetY"));
        lineEditOffsetY->setGeometry(QRect(90, 40, 131, 20));
        lineEditOffsetZ = new QLineEdit(PLYOptionDialog);
        lineEditOffsetZ->setObjectName(QStringLiteral("lineEditOffsetZ"));
        lineEditOffsetZ->setGeometry(QRect(90, 70, 131, 20));
        label_5 = new QLabel(PLYOptionDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 130, 61, 21));
        lineEditFileName = new QLineEdit(PLYOptionDialog);
        lineEditFileName->setObjectName(QStringLiteral("lineEditFileName"));
        lineEditFileName->setGeometry(QRect(90, 130, 291, 20));
        pushButtonFileName = new QPushButton(PLYOptionDialog);
        pushButtonFileName->setObjectName(QStringLiteral("pushButtonFileName"));
        pushButtonFileName->setGeometry(QRect(390, 130, 31, 21));
        pushButtonD1 = new QPushButton(PLYOptionDialog);
        pushButtonD1->setObjectName(QStringLiteral("pushButtonD1"));
        pushButtonD1->setGeometry(QRect(250, 10, 71, 31));
        pushButtonD2 = new QPushButton(PLYOptionDialog);
        pushButtonD2->setObjectName(QStringLiteral("pushButtonD2"));
        pushButtonD2->setGeometry(QRect(340, 10, 71, 31));
        pushButtonD4 = new QPushButton(PLYOptionDialog);
        pushButtonD4->setObjectName(QStringLiteral("pushButtonD4"));
        pushButtonD4->setGeometry(QRect(340, 50, 71, 31));
        pushButtonD3 = new QPushButton(PLYOptionDialog);
        pushButtonD3->setObjectName(QStringLiteral("pushButtonD3"));
        pushButtonD3->setGeometry(QRect(250, 50, 71, 31));

        retranslateUi(PLYOptionDialog);

        QMetaObject::connectSlotsByName(PLYOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *PLYOptionDialog)
    {
        PLYOptionDialog->setWindowTitle(QApplication::translate("PLYOptionDialog", "PLYOptionDialog", 0));
        label_4->setText(QApplication::translate("PLYOptionDialog", "Scale:", 0));
        pushButtonCancel->setText(QApplication::translate("PLYOptionDialog", "Cancel", 0));
        pushButtonOK->setText(QApplication::translate("PLYOptionDialog", "OK", 0));
        label_2->setText(QApplication::translate("PLYOptionDialog", "Offset Y:", 0));
        label->setText(QApplication::translate("PLYOptionDialog", "Offset X:", 0));
        label_3->setText(QApplication::translate("PLYOptionDialog", "Offset Z:", 0));
        label_5->setText(QApplication::translate("PLYOptionDialog", "File name:", 0));
        pushButtonFileName->setText(QApplication::translate("PLYOptionDialog", "...", 0));
        pushButtonD1->setText(QApplication::translate("PLYOptionDialog", "D1", 0));
        pushButtonD2->setText(QApplication::translate("PLYOptionDialog", "D2", 0));
        pushButtonD4->setText(QApplication::translate("PLYOptionDialog", "D4", 0));
        pushButtonD3->setText(QApplication::translate("PLYOptionDialog", "D3", 0));
    } // retranslateUi

};

namespace Ui {
    class PLYOptionDialog: public Ui_PLYOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLYOPTIONDIALOG_H
