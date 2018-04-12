/********************************************************************************
** Form generated from reading UI file 'TopFaceOptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TOPFACEOPTIONDIALOG_H
#define UI_TOPFACEOPTIONDIALOG_H

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

class Ui_TopFaceOptionDialog
{
public:
    QPushButton *pushButtonD3;
    QLineEdit *lineEditFileName;
    QLineEdit *lineEditOffsetZ;
    QPushButton *pushButtonFileName;
    QPushButton *pushButtonD2;
    QLabel *label_2;
    QPushButton *pushButtonOK;
    QPushButton *pushButtonD1;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBoxScale;
    QLineEdit *lineEditOffsetX;
    QLabel *label_4;
    QLineEdit *lineEditOffsetY;
    QPushButton *pushButtonD4;
    QPushButton *pushButtonCancel;
    QLabel *label;
    QLabel *label_3;

    void setupUi(QDialog *TopFaceOptionDialog)
    {
        if (TopFaceOptionDialog->objectName().isEmpty())
            TopFaceOptionDialog->setObjectName(QStringLiteral("TopFaceOptionDialog"));
        TopFaceOptionDialog->resize(422, 201);
        pushButtonD3 = new QPushButton(TopFaceOptionDialog);
        pushButtonD3->setObjectName(QStringLiteral("pushButtonD3"));
        pushButtonD3->setGeometry(QRect(240, 50, 71, 31));
        lineEditFileName = new QLineEdit(TopFaceOptionDialog);
        lineEditFileName->setObjectName(QStringLiteral("lineEditFileName"));
        lineEditFileName->setGeometry(QRect(80, 130, 291, 20));
        lineEditOffsetZ = new QLineEdit(TopFaceOptionDialog);
        lineEditOffsetZ->setObjectName(QStringLiteral("lineEditOffsetZ"));
        lineEditOffsetZ->setGeometry(QRect(80, 70, 131, 20));
        pushButtonFileName = new QPushButton(TopFaceOptionDialog);
        pushButtonFileName->setObjectName(QStringLiteral("pushButtonFileName"));
        pushButtonFileName->setGeometry(QRect(380, 130, 31, 21));
        pushButtonD2 = new QPushButton(TopFaceOptionDialog);
        pushButtonD2->setObjectName(QStringLiteral("pushButtonD2"));
        pushButtonD2->setGeometry(QRect(330, 10, 71, 31));
        label_2 = new QLabel(TopFaceOptionDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 40, 91, 21));
        pushButtonOK = new QPushButton(TopFaceOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(230, 160, 81, 31));
        pushButtonD1 = new QPushButton(TopFaceOptionDialog);
        pushButtonD1->setObjectName(QStringLiteral("pushButtonD1"));
        pushButtonD1->setGeometry(QRect(240, 10, 71, 31));
        label_5 = new QLabel(TopFaceOptionDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 130, 61, 21));
        doubleSpinBoxScale = new QDoubleSpinBox(TopFaceOptionDialog);
        doubleSpinBoxScale->setObjectName(QStringLiteral("doubleSpinBoxScale"));
        doubleSpinBoxScale->setGeometry(QRect(80, 100, 71, 22));
        lineEditOffsetX = new QLineEdit(TopFaceOptionDialog);
        lineEditOffsetX->setObjectName(QStringLiteral("lineEditOffsetX"));
        lineEditOffsetX->setGeometry(QRect(80, 10, 131, 20));
        label_4 = new QLabel(TopFaceOptionDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 100, 61, 21));
        lineEditOffsetY = new QLineEdit(TopFaceOptionDialog);
        lineEditOffsetY->setObjectName(QStringLiteral("lineEditOffsetY"));
        lineEditOffsetY->setGeometry(QRect(80, 40, 131, 20));
        pushButtonD4 = new QPushButton(TopFaceOptionDialog);
        pushButtonD4->setObjectName(QStringLiteral("pushButtonD4"));
        pushButtonD4->setGeometry(QRect(330, 50, 71, 31));
        pushButtonCancel = new QPushButton(TopFaceOptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(330, 160, 81, 31));
        label = new QLabel(TopFaceOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 10, 47, 21));
        label_3 = new QLabel(TopFaceOptionDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 70, 71, 21));

        retranslateUi(TopFaceOptionDialog);

        QMetaObject::connectSlotsByName(TopFaceOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *TopFaceOptionDialog)
    {
        TopFaceOptionDialog->setWindowTitle(QApplication::translate("TopFaceOptionDialog", "TopFaceOptionDialog", 0));
        pushButtonD3->setText(QApplication::translate("TopFaceOptionDialog", "D3", 0));
        pushButtonFileName->setText(QApplication::translate("TopFaceOptionDialog", "...", 0));
        pushButtonD2->setText(QApplication::translate("TopFaceOptionDialog", "D2", 0));
        label_2->setText(QApplication::translate("TopFaceOptionDialog", "Offset Y:", 0));
        pushButtonOK->setText(QApplication::translate("TopFaceOptionDialog", "OK", 0));
        pushButtonD1->setText(QApplication::translate("TopFaceOptionDialog", "D1", 0));
        label_5->setText(QApplication::translate("TopFaceOptionDialog", "File name:", 0));
        label_4->setText(QApplication::translate("TopFaceOptionDialog", "Scale:", 0));
        pushButtonD4->setText(QApplication::translate("TopFaceOptionDialog", "D4", 0));
        pushButtonCancel->setText(QApplication::translate("TopFaceOptionDialog", "Cancel", 0));
        label->setText(QApplication::translate("TopFaceOptionDialog", "Offset X:", 0));
        label_3->setText(QApplication::translate("TopFaceOptionDialog", "Offset Z:", 0));
    } // retranslateUi

};

namespace Ui {
    class TopFaceOptionDialog: public Ui_TopFaceOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TOPFACEOPTIONDIALOG_H
