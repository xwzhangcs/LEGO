/********************************************************************************
** Form generated from reading UI file 'DPOptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DPOPTIONDIALOG_H
#define UI_DPOPTIONDIALOG_H

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
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_DPOptionDialog
{
public:
    QLabel *label;
    QPushButton *pushButtonOK;
    QPushButton *pushButtonCancel;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBoxLayeringThreshold;
    QSpinBox *spinBoxEpsilon;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBoxSnappingThreshold;
    QLabel *label_4;
    QLineEdit *lineEditOrientation;

    void setupUi(QDialog *DPOptionDialog)
    {
        if (DPOptionDialog->objectName().isEmpty())
            DPOptionDialog->setObjectName(QStringLiteral("DPOptionDialog"));
        DPOptionDialog->resize(246, 180);
        DPOptionDialog->setMinimumSize(QSize(246, 180));
        DPOptionDialog->setMaximumSize(QSize(246, 180));
        label = new QLabel(DPOptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 47, 21));
        pushButtonOK = new QPushButton(DPOptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(30, 140, 81, 31));
        pushButtonCancel = new QPushButton(DPOptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(140, 140, 81, 31));
        label_2 = new QLabel(DPOptionDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 40, 91, 21));
        doubleSpinBoxLayeringThreshold = new QDoubleSpinBox(DPOptionDialog);
        doubleSpinBoxLayeringThreshold->setObjectName(QStringLiteral("doubleSpinBoxLayeringThreshold"));
        doubleSpinBoxLayeringThreshold->setGeometry(QRect(160, 40, 62, 22));
        spinBoxEpsilon = new QSpinBox(DPOptionDialog);
        spinBoxEpsilon->setObjectName(QStringLiteral("spinBoxEpsilon"));
        spinBoxEpsilon->setGeometry(QRect(160, 10, 61, 22));
        label_3 = new QLabel(DPOptionDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 70, 131, 21));
        doubleSpinBoxSnappingThreshold = new QDoubleSpinBox(DPOptionDialog);
        doubleSpinBoxSnappingThreshold->setObjectName(QStringLiteral("doubleSpinBoxSnappingThreshold"));
        doubleSpinBoxSnappingThreshold->setGeometry(QRect(160, 70, 62, 22));
        label_4 = new QLabel(DPOptionDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 100, 131, 21));
        lineEditOrientation = new QLineEdit(DPOptionDialog);
        lineEditOrientation->setObjectName(QStringLiteral("lineEditOrientation"));
        lineEditOrientation->setGeometry(QRect(92, 100, 131, 20));
        lineEditOrientation->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        retranslateUi(DPOptionDialog);

        QMetaObject::connectSlotsByName(DPOptionDialog);
    } // setupUi

    void retranslateUi(QDialog *DPOptionDialog)
    {
        DPOptionDialog->setWindowTitle(QApplication::translate("DPOptionDialog", "Option Dialog", Q_NULLPTR));
        label->setText(QApplication::translate("DPOptionDialog", "Epsilon:", Q_NULLPTR));
        pushButtonOK->setText(QApplication::translate("DPOptionDialog", "OK", Q_NULLPTR));
        pushButtonCancel->setText(QApplication::translate("DPOptionDialog", "Cancel", Q_NULLPTR));
        label_2->setText(QApplication::translate("DPOptionDialog", "Layering threshold:", Q_NULLPTR));
        label_3->setText(QApplication::translate("DPOptionDialog", "Snap-to-vertex threshold:", Q_NULLPTR));
        label_4->setText(QApplication::translate("DPOptionDialog", "Orientation:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class DPOptionDialog: public Ui_DPOptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DPOPTIONDIALOG_H
