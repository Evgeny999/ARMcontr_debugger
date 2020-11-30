/********************************************************************************
** Form generated from reading UI file 'celllist1.ui'
**
** Created: Mon 9. Sep 14:40:37 2019
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CELLLIST1_H
#define UI_CELLLIST1_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CellList1
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QTableWidget *Table;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *CellList1)
    {
        if (CellList1->objectName().isEmpty())
            CellList1->setObjectName(QString::fromUtf8("CellList1"));
        CellList1->resize(797, 599);
        centralwidget = new QWidget(CellList1);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        Table = new QTableWidget(centralwidget);
        Table->setObjectName(QString::fromUtf8("Table"));

        gridLayout->addWidget(Table, 0, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

        CellList1->setCentralWidget(centralwidget);
        menubar = new QMenuBar(CellList1);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 797, 20));
        CellList1->setMenuBar(menubar);
        statusbar = new QStatusBar(CellList1);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        CellList1->setStatusBar(statusbar);

        retranslateUi(CellList1);

        QMetaObject::connectSlotsByName(CellList1);
    } // setupUi

    void retranslateUi(QMainWindow *CellList1)
    {
        CellList1->setWindowTitle(QApplication::translate("CellList1", "MainWindow", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CellList1: public Ui_CellList1 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CELLLIST1_H
