/********************************************************************************
** Form generated from reading UI file 'registerswindow.ui'
**
** Created: Mon 9. Sep 14:40:37 2019
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTERSWINDOW_H
#define UI_REGISTERSWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RegistersWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QTableWidget *registersTable;
    QComboBox *comboBox;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *RegistersWindow)
    {
        if (RegistersWindow->objectName().isEmpty())
            RegistersWindow->setObjectName(QString::fromUtf8("RegistersWindow"));
        RegistersWindow->resize(370, 424);
        centralwidget = new QWidget(RegistersWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        registersTable = new QTableWidget(centralwidget);
        registersTable->setObjectName(QString::fromUtf8("registersTable"));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        registersTable->setFont(font);

        gridLayout->addWidget(registersTable, 1, 0, 1, 1);

        comboBox = new QComboBox(centralwidget);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        gridLayout->addWidget(comboBox, 0, 0, 1, 1);

        RegistersWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(RegistersWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 370, 20));
        RegistersWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(RegistersWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        RegistersWindow->setStatusBar(statusbar);

        retranslateUi(RegistersWindow);

        QMetaObject::connectSlotsByName(RegistersWindow);
    } // setupUi

    void retranslateUi(QMainWindow *RegistersWindow)
    {
        RegistersWindow->setWindowTitle(QApplication::translate("RegistersWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RegistersWindow: public Ui_RegistersWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTERSWINDOW_H
