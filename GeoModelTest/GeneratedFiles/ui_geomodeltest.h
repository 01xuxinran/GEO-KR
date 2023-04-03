/********************************************************************************
** Form generated from reading UI file 'geomodeltest.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GEOMODELTEST_H
#define UI_GEOMODELTEST_H

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

class Ui_GeoModelTestClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *GeoModelTestClass)
    {
        if (GeoModelTestClass->objectName().isEmpty())
            GeoModelTestClass->setObjectName(QStringLiteral("GeoModelTestClass"));
        GeoModelTestClass->resize(600, 400);
        menuBar = new QMenuBar(GeoModelTestClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        GeoModelTestClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(GeoModelTestClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        GeoModelTestClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(GeoModelTestClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        GeoModelTestClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(GeoModelTestClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        GeoModelTestClass->setStatusBar(statusBar);

        retranslateUi(GeoModelTestClass);

        QMetaObject::connectSlotsByName(GeoModelTestClass);
    } // setupUi

    void retranslateUi(QMainWindow *GeoModelTestClass)
    {
        GeoModelTestClass->setWindowTitle(QApplication::translate("GeoModelTestClass", "GeoModelTest", 0));
    } // retranslateUi

};

namespace Ui {
    class GeoModelTestClass: public Ui_GeoModelTestClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GEOMODELTEST_H
