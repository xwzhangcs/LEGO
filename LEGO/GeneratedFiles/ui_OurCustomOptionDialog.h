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
    QLabel *label_4;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBoxSnapVertexThreshold;
    QDoubleSpinBox *doubleSpinBoxSnapEdgeThreshold;

    void setupUi(QDialog *OurCustomOptionDialog)
    {
        if (OurCustomOptionDialog->objectName().isEmpty())
            OurCustomOptionDialog->setObjectName(QStringLiteral("OurCustomOptionDialog"));
        OurCustomOptionDialog->resize(246, 172);
        OurCustomOptionDialog->setMinimumSize(QSize(246, 172));
        OurCustomOptionDialog->setMaximumSize(QSize(246, 172));
        label = new QLabel(OurCustomOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 61, 21));
        spinBoxResolution = new QSpinBox(OurCustomOptionDialog);
        spinBoxResolution->setObjectName(QStringLiteral("spinBoxResolution"));
        spinBoxResolution->setGeometry(QRect(160, 10, 61, 22));
        pushButtonCancel = new QPushButton(OurCustomOptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(130, 130, 81, 31));
        pushButtonOK = new QPushButton(OurCustomOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(30, 130, 81, 31));
        doubleSpinBoxLayeringThreshold = new QDoubleSpinBox(OurCustomOptionDialog);
        doubleSpinBoxLayeringThreshold->setObjectName(QStringLiteral("doubleSpinBoxLayeringThreshold"));
        doubleSpinBoxLayeringThreshold->setGeometry(QRect(160, 40, 62, 22));
        label_2 = new QLabel(OurCustomOptionDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 40, 91, 21));
        label_4 = new QLabel(OurCustomOptionDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 100, 131, 21));
        label_3 = new QLabel(OurCustomOptionDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 70, 131, 21));
        doubleSpinBoxSnapVertexThreshold = new QDoubleSpinBox(OurCustomOptionDialog);
        doubleSpinBoxSnapVertexThreshold->setObjectName(QStringLiteral("doubleSpinBoxSnapVertexThreshold"));
        doubleSpinBoxSnapVertexThreshold->setGeometry(QRect(160, 70, 62, 22));
        doubleSpinBoxSnapEdgeThreshold = new QDoubleSpinBox(OurCustomOptionDialog);
        doubleSpinBoxSnapEdgeThreshold->setObjectName(QStringLiteral("doubleSpinBoxSnapEdgeThreshold"));
        doubleSpinBoxSnapEdgeThreshold->setGeometry(QRect(160, 100, 62, 22));

        retranslateUi(OurCustomOptionDialog);

        QMetaObject::connectSlotsByName(OurCustomOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *OurCustomOptionDialog)
    {
        OurCustomOptionDialog->setWindowTitle(QApplication::translate("OurCustomOptionDialog", "Option Dialog", 0));
        label->setText(QApplication::translate("OurCustomOptionDialog", "Resolution:", 0));
        pushButtonCancel->setText(QApplication::translate("OurCustomOptionDialog", "Cancel", 0));
        pushButtonOK->setText(QApplication::translate("OurCustomOptionDialog", "OK", 0));
        label_2->setText(QApplication::translate("OurCustomOptionDialog", "Layering threshold:", 0));
        label_4->setText(QApplication::translate("OurCustomOptionDialog", "Snap-to-edge threshold:", 0));
        label_3->setText(QApplication::translate("OurCustomOptionDialog", "Snap-to-vertex threshold:", 0));
    } // retranslateUi

};

namespace Ui {
    class OurCustomOptionDialog: public Ui_OurCustomOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OURCUSTOMOPTIONDIALOG_H
