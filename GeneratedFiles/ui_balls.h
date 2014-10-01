/********************************************************************************
** Form generated from reading UI file 'balls.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BALLS_H
#define UI_BALLS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BallsClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *BallsClass)
    {
        if (BallsClass->objectName().isEmpty())
            BallsClass->setObjectName(QStringLiteral("BallsClass"));
        BallsClass->resize(600, 400);
        menuBar = new QMenuBar(BallsClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        BallsClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(BallsClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        BallsClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(BallsClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        BallsClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(BallsClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        BallsClass->setStatusBar(statusBar);

        retranslateUi(BallsClass);

        QMetaObject::connectSlotsByName(BallsClass);
    } // setupUi

    void retranslateUi(QMainWindow *BallsClass)
    {
        BallsClass->setWindowTitle(QApplication::translate("BallsClass", "Balls", 0));
    } // retranslateUi

};

namespace Ui {
    class BallsClass: public Ui_BallsClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BALLS_H
