/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QAction *actionExit;
    QAction *actionOpenSliceImage;
    QAction *actionRenderingBasic;
    QAction *actionRenderingSSAO;
    QAction *actionRenderingHatching;
    QAction *actionOpen;
    QAction *actionSaveImage;
    QAction *actionSimplifyByOpenCV;
    QAction *actionSimplifyByOurCustom;
    QAction *actionInputVoxel;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuRendering;
    QMenu *menuSimplify;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QStringLiteral("MainWindowClass"));
        MainWindowClass->resize(752, 735);
        actionExit = new QAction(MainWindowClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionOpenSliceImage = new QAction(MainWindowClass);
        actionOpenSliceImage->setObjectName(QStringLiteral("actionOpenSliceImage"));
        QIcon icon;
        icon.addFile(QStringLiteral("Resources/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpenSliceImage->setIcon(icon);
        actionRenderingBasic = new QAction(MainWindowClass);
        actionRenderingBasic->setObjectName(QStringLiteral("actionRenderingBasic"));
        actionRenderingBasic->setCheckable(true);
        actionRenderingBasic->setChecked(true);
        actionRenderingSSAO = new QAction(MainWindowClass);
        actionRenderingSSAO->setObjectName(QStringLiteral("actionRenderingSSAO"));
        actionRenderingSSAO->setCheckable(true);
        actionRenderingHatching = new QAction(MainWindowClass);
        actionRenderingHatching->setObjectName(QStringLiteral("actionRenderingHatching"));
        actionRenderingHatching->setCheckable(true);
        actionOpen = new QAction(MainWindowClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionOpen->setIcon(icon);
        actionSaveImage = new QAction(MainWindowClass);
        actionSaveImage->setObjectName(QStringLiteral("actionSaveImage"));
        QIcon icon1;
        icon1.addFile(QStringLiteral("Resources/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSaveImage->setIcon(icon1);
        actionSimplifyByOpenCV = new QAction(MainWindowClass);
        actionSimplifyByOpenCV->setObjectName(QStringLiteral("actionSimplifyByOpenCV"));
        actionSimplifyByOurCustom = new QAction(MainWindowClass);
        actionSimplifyByOurCustom->setObjectName(QStringLiteral("actionSimplifyByOurCustom"));
        actionInputVoxel = new QAction(MainWindowClass);
        actionInputVoxel->setObjectName(QStringLiteral("actionInputVoxel"));
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 752, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuRendering = new QMenu(menuBar);
        menuRendering->setObjectName(QStringLiteral("menuRendering"));
        menuSimplify = new QMenu(menuBar);
        menuSimplify->setObjectName(QStringLiteral("menuSimplify"));
        MainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindowClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuSimplify->menuAction());
        menuBar->addAction(menuRendering->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addSeparator();
        menuFile->addAction(actionSaveImage);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuRendering->addAction(actionRenderingBasic);
        menuRendering->addAction(actionRenderingSSAO);
        menuRendering->addAction(actionRenderingHatching);
        menuSimplify->addAction(actionInputVoxel);
        menuSimplify->addAction(actionSimplifyByOpenCV);
        menuSimplify->addAction(actionSimplifyByOurCustom);

        retranslateUi(MainWindowClass);

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "MainWindow", 0));
        actionExit->setText(QApplication::translate("MainWindowClass", "Exit", 0));
        actionOpenSliceImage->setText(QApplication::translate("MainWindowClass", "Open Slice Image", 0));
        actionOpenSliceImage->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+O", 0));
        actionRenderingBasic->setText(QApplication::translate("MainWindowClass", "Basic", 0));
        actionRenderingSSAO->setText(QApplication::translate("MainWindowClass", "SSAO", 0));
        actionRenderingHatching->setText(QApplication::translate("MainWindowClass", "Hatching", 0));
        actionOpen->setText(QApplication::translate("MainWindowClass", "Open", 0));
        actionOpen->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+O", 0));
        actionSaveImage->setText(QApplication::translate("MainWindowClass", "Save Image", 0));
        actionSaveImage->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+S", 0));
        actionSimplifyByOpenCV->setText(QApplication::translate("MainWindowClass", "OpenCV", 0));
        actionSimplifyByOurCustom->setText(QApplication::translate("MainWindowClass", "Custom", 0));
        actionInputVoxel->setText(QApplication::translate("MainWindowClass", "Input", 0));
        menuFile->setTitle(QApplication::translate("MainWindowClass", "File", 0));
        menuRendering->setTitle(QApplication::translate("MainWindowClass", "Rendering", 0));
        menuSimplify->setTitle(QApplication::translate("MainWindowClass", "Simplify", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
