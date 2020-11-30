/********************************************************************************
** Form generated from reading UI file 'setaddwindow.ui'
**
** Created: Mon 9. Sep 14:45:21 2019
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETADDWINDOW_H
#define UI_SETADDWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_setAddWindow
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *lineEdit;
    QPushButton *pushButton;

    void setupUi(QDialog *setAddWindow)
    {
        if (setAddWindow->objectName().isEmpty())
            setAddWindow->setObjectName(QString::fromUtf8("setAddWindow"));
        setAddWindow->resize(224, 123);
        setAddWindow->setMaximumSize(QSize(440, 230));
        gridLayout = new QGridLayout(setAddWindow);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(setAddWindow);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        label_3 = new QLabel(setAddWindow);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout->addWidget(label_3);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(setAddWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        lineEdit = new QLineEdit(setAddWindow);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setEnabled(true);

        horizontalLayout_2->addWidget(lineEdit);


        verticalLayout->addLayout(horizontalLayout_2);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

        pushButton = new QPushButton(setAddWindow);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        gridLayout->addWidget(pushButton, 1, 0, 1, 1);


        retranslateUi(setAddWindow);

        QMetaObject::connectSlotsByName(setAddWindow);
    } // setupUi

    void retranslateUi(QDialog *setAddWindow)
    {
        setAddWindow->setWindowTitle(QApplication::translate("setAddWindow", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("setAddWindow", "Address:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QString());
        label_2->setText(QApplication::translate("setAddWindow", "NewValue:", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("setAddWindow", "Ok", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class setAddWindow: public Ui_setAddWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETADDWINDOW_H
