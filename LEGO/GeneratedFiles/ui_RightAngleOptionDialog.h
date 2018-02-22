/********************************************************************************
** Form generated from reading UI file 'RightAngleOptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RIGHTANGLEOPTIONDIALOG_H
#define UI_RIGHTANGLEOPTIONDIALOG_H

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

class Ui_RightAngleOptionDialog
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

    void setupUi(QDialog *RightAngleOptionDialog)
    {
        if (RightAngleOptionDialog->objectName().isEmpty())
            RightAngleOptionDialog->setObjectName(QStringLiteral("RightAngleOptionDialog"));
        RightAngleOptionDialog->resize(246, 172);
        RightAngleOptionDialog->setMinimumSize(QSize(246, 172));
        RightAngleOptionDialog->setMaximumSize(QSize(246, 172));
        label = new QLabel(RightAngleOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 61, 21));
        spinBoxResolution = new QSpinBox(RightAngleOptionDialog);
        spinBoxResolution->setObjectName(QStringLiteral("spinBoxResolution"));
        spinBoxResolution->setGeometry(QRect(160, 10, 61, 22));
        pushButtonCancel = new QPushButton(RightAngleOptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(130, 130, 81, 31));
        pushButtonOK = new QPushButton(RightAngleOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(30, 130, 81, 31));
        doubleSpinBoxLayeringThreshold = new QDoubleSpinBox(RightAngleOptionDialog);
        doubleSpinBoxLayeringThreshold->setObjectName(QStringLiteral("doubleSpinBoxLayeringThreshold"));
        doubleSpinBoxLayeringThreshold->setGeometry(QRect(160, 40, 62, 22));
        label_2 = new QLabel(RightAngleOptionDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 40, 91, 21));
        label_4 = new QLabel(RightAngleOptionDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 100, 131, 21));
        label_3 = new QLabel(RightAngleOptionDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 70, 131, 21));
        doubleSpinBoxSnapVertexThreshold = new QDoubleSpinBox(RightAngleOptionDialog);
        doubleSpinBoxSnapVertexThreshold->setObjectName(QStringLiteral("doubleSpinBoxSnapVertexThreshold"));
        doubleSpinBoxSnapVertexThreshold->setGeometry(QRect(160, 70, 62, 22));
        doubleSpinBoxSnapEdgeThreshold = new QDoubleSpinBox(RightAngleOptionDialog);
        doubleSpinBoxSnapEdgeThreshold->setObjectName(QStringLiteral("doubleSpinBoxSnapEdgeThreshold"));
        doubleSpinBoxSnapEdgeThreshold->setGeometry(QRect(160, 100, 62, 22));

        retranslateUi(RightAngleOptionDialog);

        QMetaObject::connectSlotsByName(RightAngleOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *RightAngleOptionDialog)
    {
        RightAngleOptionDialog->setWindowTitle(QApplication::translate("RightAngleOptionDialog", "Option Dialog", 0));
        label->setText(QApplication::translate("RightAngleOptionDialog", "Resolution:", 0));
        pushButtonCancel->setText(QApplication::translate("RightAngleOptionDialog", "Cancel", 0));
        pushButtonOK->setText(QApplication::translate("RightAngleOptionDialog", "OK", 0));
        label_2->setText(QApplication::translate("RightAngleOptionDialog", "Layering threshold:", 0));
        label_4->setText(QApplication::translate("RightAngleOptionDialog", "Snap-to-edge threshold:", 0));
        label_3->setText(QApplication::translate("RightAngleOptionDialog", "Snap-to-vertex threshold:", 0));
    } // retranslateUi

};

namespace Ui {
    class RightAngleOptionDialog: public Ui_RightAngleOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RIGHTANGLEOPTIONDIALOG_H
