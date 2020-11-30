/********************************************************************************
** Form generated from reading UI file 'alotofwindow.ui'
**
** Created: Sun 21. Apr 17:58:34 2019
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALOTOFWINDOW_H
#define UI_ALOTOFWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ALotOfWindow
{
public:
    QMenuBar *menubar;
    QWidget *centralwidget;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ALotOfWindow)
    {
        if (ALotOfWindow->objectName().isEmpty())
            ALotOfWindow->setObjectName(QString::fromUtf8("ALotOfWindow"));
        ALotOfWindow->resize(800, 600);
        menubar = new QMenuBar(ALotOfWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        ALotOfWindow->setMenuBar(menubar);
        centralwidget = new QWidget(ALotOfWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        ALotOfWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(ALotOfWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ALotOfWindow->setStatusBar(statusbar);

        retranslateUi(ALotOfWindow);

        QMetaObject::connectSlotsByName(ALotOfWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ALotOfWindow)
    {
        ALotOfWindow->setWindowTitle(QApplication::translate("ALotOfWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ALotOfWindow: public Ui_ALotOfWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALOTOFWINDOW_H
