/********************************************************************************
** Form generated from reading UI file 'celllist.ui'
**
** Created: Tue 21. May 13:26:59 2019
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CELLLIST_H
#define UI_CELLLIST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CellList
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QTableWidget *Table;

    void setupUi(QWidget *CellList)
    {
        if (CellList->objectName().isEmpty())
            CellList->setObjectName(QString::fromUtf8("CellList"));
        CellList->resize(499, 388);
        gridLayout_2 = new QGridLayout(CellList);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        Table = new QTableWidget(CellList);
        Table->setObjectName(QString::fromUtf8("Table"));

        gridLayout->addWidget(Table, 0, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        retranslateUi(CellList);

        QMetaObject::connectSlotsByName(CellList);
    } // setupUi

    void retranslateUi(QWidget *CellList)
    {
        CellList->setWindowTitle(QApplication::translate("CellList", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CellList: public Ui_CellList {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CELLLIST_H
