/********************************************************************************
** Form generated from reading UI file 'cellvaluechangewindow.ui'
**
** Created: Mon 9. Sep 14:40:37 2019
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CELLVALUECHANGEWINDOW_H
#define UI_CELLVALUECHANGEWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>

QT_BEGIN_NAMESPACE

class Ui_CellValueChangeWindow
{
public:
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QSplitter *splitter;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label;

    void setupUi(QDialog *CellValueChangeWindow)
    {
        if (CellValueChangeWindow->objectName().isEmpty())
            CellValueChangeWindow->setObjectName(QString::fromUtf8("CellValueChangeWindow"));
        CellValueChangeWindow->resize(214, 187);
        lineEdit = new QLineEdit(CellValueChangeWindow);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(10, 100, 201, 31));
        pushButton = new QPushButton(CellValueChangeWindow);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(40, 140, 91, 41));
        splitter = new QSplitter(CellValueChangeWindow);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setGeometry(QRect(10, 10, 201, 91));
        splitter->setOrientation(Qt::Vertical);
        label_2 = new QLabel(splitter);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        splitter->addWidget(label_2);
        label_3 = new QLabel(splitter);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        splitter->addWidget(label_3);
        label = new QLabel(splitter);
        label->setObjectName(QString::fromUtf8("label"));
        splitter->addWidget(label);

        retranslateUi(CellValueChangeWindow);

        QMetaObject::connectSlotsByName(CellValueChangeWindow);
    } // setupUi

    void retranslateUi(QDialog *CellValueChangeWindow)
    {
        CellValueChangeWindow->setWindowTitle(QApplication::translate("CellValueChangeWindow", "Dialog", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("CellValueChangeWindow", "OK", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("CellValueChangeWindow", "Address:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("CellValueChangeWindow", "NewValue", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("CellValueChangeWindow", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CellValueChangeWindow: public Ui_CellValueChangeWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CELLVALUECHANGEWINDOW_H
