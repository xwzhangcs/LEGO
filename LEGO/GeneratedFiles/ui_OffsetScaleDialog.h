/********************************************************************************
** Form generated from reading UI file 'OffsetScaleDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OFFSETSCALEDIALOG_H
#define UI_OFFSETSCALEDIALOG_H

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

class Ui_OffsetScaleDialog
{
public:
    QPushButton *pushButtonD3;
    QPushButton *pushButtonD1;
    QDoubleSpinBox *doubleSpinBoxScale;
    QLabel *label_2;
    QPushButton *pushButtonOK;
    QPushButton *pushButtonD2;
    QLabel *label;
    QLineEdit *lineEditOffsetX;
    QLabel *label_4;
    QLineEdit *lineEditOffsetY;
    QLineEdit *lineEditOffsetZ;
    QPushButton *pushButtonD4;
    QPushButton *pushButtonCancel;
    QLabel *label_3;

    void setupUi(QDialog *OffsetScaleDialog)
    {
        if (OffsetScaleDialog->objectName().isEmpty())
            OffsetScaleDialog->setObjectName(QStringLiteral("OffsetScaleDialog"));
        OffsetScaleDialog->resize(410, 169);
        pushButtonD3 = new QPushButton(OffsetScaleDialog);
        pushButtonD3->setObjectName(QStringLiteral("pushButtonD3"));
        pushButtonD3->setGeometry(QRect(290, 50, 51, 31));
        pushButtonD1 = new QPushButton(OffsetScaleDialog);
        pushButtonD1->setObjectName(QStringLiteral("pushButtonD1"));
        pushButtonD1->setGeometry(QRect(290, 10, 51, 31));
        doubleSpinBoxScale = new QDoubleSpinBox(OffsetScaleDialog);
        doubleSpinBoxScale->setObjectName(QStringLiteral("doubleSpinBoxScale"));
        doubleSpinBoxScale->setGeometry(QRect(80, 100, 71, 22));
        label_2 = new QLabel(OffsetScaleDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 40, 91, 21));
        pushButtonOK = new QPushButton(OffsetScaleDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(220, 130, 81, 31));
        pushButtonD2 = new QPushButton(OffsetScaleDialog);
        pushButtonD2->setObjectName(QStringLiteral("pushButtonD2"));
        pushButtonD2->setGeometry(QRect(350, 10, 51, 31));
        label = new QLabel(OffsetScaleDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 10, 47, 21));
        lineEditOffsetX = new QLineEdit(OffsetScaleDialog);
        lineEditOffsetX->setObjectName(QStringLiteral("lineEditOffsetX"));
        lineEditOffsetX->setGeometry(QRect(80, 10, 191, 20));
        label_4 = new QLabel(OffsetScaleDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 100, 61, 21));
        lineEditOffsetY = new QLineEdit(OffsetScaleDialog);
        lineEditOffsetY->setObjectName(QStringLiteral("lineEditOffsetY"));
        lineEditOffsetY->setGeometry(QRect(80, 40, 191, 20));
        lineEditOffsetZ = new QLineEdit(OffsetScaleDialog);
        lineEditOffsetZ->setObjectName(QStringLiteral("lineEditOffsetZ"));
        lineEditOffsetZ->setGeometry(QRect(80, 70, 191, 20));
        pushButtonD4 = new QPushButton(OffsetScaleDialog);
        pushButtonD4->setObjectName(QStringLiteral("pushButtonD4"));
        pushButtonD4->setGeometry(QRect(350, 50, 51, 31));
        pushButtonCancel = new QPushButton(OffsetScaleDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(320, 130, 81, 31));
        label_3 = new QLabel(OffsetScaleDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 70, 71, 21));

        retranslateUi(OffsetScaleDialog);

        QMetaObject::connectSlotsByName(OffsetScaleDialog);
    } // setupUi

    void retranslateUi(QDialog *OffsetScaleDialog)
    {
        OffsetScaleDialog->setWindowTitle(QApplication::translate("OffsetScaleDialog", "OffsetScaleDialog", Q_NULLPTR));
        pushButtonD3->setText(QApplication::translate("OffsetScaleDialog", "D3", Q_NULLPTR));
        pushButtonD1->setText(QApplication::translate("OffsetScaleDialog", "D1", Q_NULLPTR));
        label_2->setText(QApplication::translate("OffsetScaleDialog", "Offset Y:", Q_NULLPTR));
        pushButtonOK->setText(QApplication::translate("OffsetScaleDialog", "OK", Q_NULLPTR));
        pushButtonD2->setText(QApplication::translate("OffsetScaleDialog", "D2", Q_NULLPTR));
        label->setText(QApplication::translate("OffsetScaleDialog", "Offset X:", Q_NULLPTR));
        label_4->setText(QApplication::translate("OffsetScaleDialog", "Scale:", Q_NULLPTR));
        pushButtonD4->setText(QApplication::translate("OffsetScaleDialog", "D4", Q_NULLPTR));
        pushButtonCancel->setText(QApplication::translate("OffsetScaleDialog", "Cancel", Q_NULLPTR));
        label_3->setText(QApplication::translate("OffsetScaleDialog", "Offset Z:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OffsetScaleDialog: public Ui_OffsetScaleDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OFFSETSCALEDIALOG_H
