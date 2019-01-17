/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
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
    QAction *actionSavePLY;
    QAction *actionSimplifyByDP;
    QAction *actionSimplifyByRightAngle;
    QAction *actionInputVoxel;
    QAction *actionSaveImage;
    QAction *actionSaveImageHD;
    QAction *actionColor;
    QAction *actionTexture;
    QAction *actionColoringByLayer;
    QAction *actionSimplifyByCurve;
    QAction *actionSimplifyByAll;
    QAction *actionDPTest;
    QAction *actionRightAngleTest;
    QAction *actionCurveTest;
    QAction *actionSimplifyByCurveRightAngle;
    QAction *actionSaveOBJ;
    QAction *actionSaveTopFaces;
    QAction *actionOffsetScale;
    QAction *actionSimplifyByEfficientRANSAC;
    QAction *actionGenerateFacadeImages;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuRendering;
    QMenu *menuSimplify;
    QMenu *menuTools;
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
        actionSavePLY = new QAction(MainWindowClass);
        actionSavePLY->setObjectName(QStringLiteral("actionSavePLY"));
        actionSimplifyByDP = new QAction(MainWindowClass);
        actionSimplifyByDP->setObjectName(QStringLiteral("actionSimplifyByDP"));
        actionSimplifyByDP->setCheckable(true);
        actionSimplifyByRightAngle = new QAction(MainWindowClass);
        actionSimplifyByRightAngle->setObjectName(QStringLiteral("actionSimplifyByRightAngle"));
        actionSimplifyByRightAngle->setCheckable(true);
        actionInputVoxel = new QAction(MainWindowClass);
        actionInputVoxel->setObjectName(QStringLiteral("actionInputVoxel"));
        actionInputVoxel->setCheckable(true);
        actionInputVoxel->setChecked(true);
        actionSaveImage = new QAction(MainWindowClass);
        actionSaveImage->setObjectName(QStringLiteral("actionSaveImage"));
        actionSaveImageHD = new QAction(MainWindowClass);
        actionSaveImageHD->setObjectName(QStringLiteral("actionSaveImageHD"));
        actionColor = new QAction(MainWindowClass);
        actionColor->setObjectName(QStringLiteral("actionColor"));
        actionColor->setCheckable(true);
        actionColor->setChecked(true);
        actionTexture = new QAction(MainWindowClass);
        actionTexture->setObjectName(QStringLiteral("actionTexture"));
        actionTexture->setCheckable(true);
        actionColoringByLayer = new QAction(MainWindowClass);
        actionColoringByLayer->setObjectName(QStringLiteral("actionColoringByLayer"));
        actionColoringByLayer->setCheckable(true);
        actionSimplifyByCurve = new QAction(MainWindowClass);
        actionSimplifyByCurve->setObjectName(QStringLiteral("actionSimplifyByCurve"));
        actionSimplifyByCurve->setCheckable(true);
        actionSimplifyByAll = new QAction(MainWindowClass);
        actionSimplifyByAll->setObjectName(QStringLiteral("actionSimplifyByAll"));
        actionSimplifyByAll->setCheckable(true);
        actionDPTest = new QAction(MainWindowClass);
        actionDPTest->setObjectName(QStringLiteral("actionDPTest"));
        actionRightAngleTest = new QAction(MainWindowClass);
        actionRightAngleTest->setObjectName(QStringLiteral("actionRightAngleTest"));
        actionCurveTest = new QAction(MainWindowClass);
        actionCurveTest->setObjectName(QStringLiteral("actionCurveTest"));
        actionSimplifyByCurveRightAngle = new QAction(MainWindowClass);
        actionSimplifyByCurveRightAngle->setObjectName(QStringLiteral("actionSimplifyByCurveRightAngle"));
        actionSimplifyByCurveRightAngle->setCheckable(true);
        actionSaveOBJ = new QAction(MainWindowClass);
        actionSaveOBJ->setObjectName(QStringLiteral("actionSaveOBJ"));
        QIcon icon1;
        icon1.addFile(QStringLiteral("Resources/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSaveOBJ->setIcon(icon1);
        actionSaveTopFaces = new QAction(MainWindowClass);
        actionSaveTopFaces->setObjectName(QStringLiteral("actionSaveTopFaces"));
        actionOffsetScale = new QAction(MainWindowClass);
        actionOffsetScale->setObjectName(QStringLiteral("actionOffsetScale"));
        actionSimplifyByEfficientRANSAC = new QAction(MainWindowClass);
        actionSimplifyByEfficientRANSAC->setObjectName(QStringLiteral("actionSimplifyByEfficientRANSAC"));
        actionGenerateFacadeImages = new QAction(MainWindowClass);
        actionGenerateFacadeImages->setObjectName(QStringLiteral("actionGenerateFacadeImages"));
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
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName(QStringLiteral("menuTools"));
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
        menuBar->addAction(menuTools->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSaveOBJ);
        menuFile->addAction(actionSaveTopFaces);
        menuFile->addAction(actionSavePLY);
        menuFile->addSeparator();
        menuFile->addAction(actionSaveImage);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuRendering->addAction(actionColor);
        menuRendering->addAction(actionTexture);
        menuRendering->addSeparator();
        menuRendering->addAction(actionRenderingBasic);
        menuRendering->addAction(actionRenderingSSAO);
        menuRendering->addAction(actionRenderingHatching);
        menuSimplify->addAction(actionInputVoxel);
        menuSimplify->addSeparator();
        menuSimplify->addAction(actionSimplifyByAll);
        menuSimplify->addAction(actionSimplifyByDP);
        menuSimplify->addAction(actionSimplifyByRightAngle);
        menuSimplify->addAction(actionSimplifyByCurve);
        menuSimplify->addAction(actionSimplifyByCurveRightAngle);
        menuSimplify->addAction(actionSimplifyByEfficientRANSAC);
        menuSimplify->addSeparator();
        menuSimplify->addAction(actionDPTest);
        menuSimplify->addAction(actionRightAngleTest);
        menuSimplify->addAction(actionCurveTest);
        menuSimplify->addSeparator();
        menuSimplify->addAction(actionOffsetScale);
        menuTools->addAction(actionGenerateFacadeImages);

        retranslateUi(MainWindowClass);

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "LEGO", Q_NULLPTR));
        actionExit->setText(QApplication::translate("MainWindowClass", "Exit", Q_NULLPTR));
        actionOpenSliceImage->setText(QApplication::translate("MainWindowClass", "Open Slice Image", Q_NULLPTR));
        actionOpenSliceImage->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+O", Q_NULLPTR));
        actionRenderingBasic->setText(QApplication::translate("MainWindowClass", "Basic", Q_NULLPTR));
        actionRenderingSSAO->setText(QApplication::translate("MainWindowClass", "SSAO", Q_NULLPTR));
        actionRenderingHatching->setText(QApplication::translate("MainWindowClass", "Hatching", Q_NULLPTR));
        actionOpen->setText(QApplication::translate("MainWindowClass", "Open", Q_NULLPTR));
        actionOpen->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+O", Q_NULLPTR));
        actionSavePLY->setText(QApplication::translate("MainWindowClass", "Save PLY", Q_NULLPTR));
        actionSimplifyByDP->setText(QApplication::translate("MainWindowClass", "Douglas-Peucker", Q_NULLPTR));
        actionSimplifyByRightAngle->setText(QApplication::translate("MainWindowClass", "Right Angle", Q_NULLPTR));
        actionInputVoxel->setText(QApplication::translate("MainWindowClass", "Input", Q_NULLPTR));
        actionSaveImage->setText(QApplication::translate("MainWindowClass", "Save Image", Q_NULLPTR));
        actionSaveImage->setShortcut(QApplication::translate("MainWindowClass", "P", Q_NULLPTR));
        actionSaveImageHD->setText(QApplication::translate("MainWindowClass", "Save Image HD", Q_NULLPTR));
        actionColor->setText(QApplication::translate("MainWindowClass", "Color", Q_NULLPTR));
        actionTexture->setText(QApplication::translate("MainWindowClass", "Texture", Q_NULLPTR));
        actionColoringByLayer->setText(QApplication::translate("MainWindowClass", "Coloring by Layer", Q_NULLPTR));
        actionSimplifyByCurve->setText(QApplication::translate("MainWindowClass", "Curve", Q_NULLPTR));
        actionSimplifyByAll->setText(QApplication::translate("MainWindowClass", "All", Q_NULLPTR));
        actionDPTest->setText(QApplication::translate("MainWindowClass", "Douglas-Peucker Test", Q_NULLPTR));
        actionRightAngleTest->setText(QApplication::translate("MainWindowClass", "Right Angle Test", Q_NULLPTR));
        actionCurveTest->setText(QApplication::translate("MainWindowClass", "Curve Test", Q_NULLPTR));
        actionSimplifyByCurveRightAngle->setText(QApplication::translate("MainWindowClass", "Curve + Right Angle", Q_NULLPTR));
        actionSaveOBJ->setText(QApplication::translate("MainWindowClass", "Save OBJ", Q_NULLPTR));
        actionSaveOBJ->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+S", Q_NULLPTR));
        actionSaveTopFaces->setText(QApplication::translate("MainWindowClass", "Save Top Faces", Q_NULLPTR));
        actionOffsetScale->setText(QApplication::translate("MainWindowClass", "Offset & Scale", Q_NULLPTR));
        actionSimplifyByEfficientRANSAC->setText(QApplication::translate("MainWindowClass", "EfficientRANSAC", Q_NULLPTR));
        actionGenerateFacadeImages->setText(QApplication::translate("MainWindowClass", "Generate Facade Images", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("MainWindowClass", "File", Q_NULLPTR));
        menuRendering->setTitle(QApplication::translate("MainWindowClass", "Rendering", Q_NULLPTR));
        menuSimplify->setTitle(QApplication::translate("MainWindowClass", "Simplify", Q_NULLPTR));
        menuTools->setTitle(QApplication::translate("MainWindowClass", "Tools", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
