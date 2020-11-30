/********************************************************************************
** Form generated from reading UI file 'fileopenerrorwindow.ui'
**
** Created: Mon 9. Sep 14:40:37 2019
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILEOPENERRORWINDOW_H
#define UI_FILEOPENERRORWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_FileOpenErrorWindow
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QLabel *label;
    QTextEdit *textEdit;
    QLabel *label_2;

    void setupUi(QDialog *FileOpenErrorWindow)
    {
        if (FileOpenErrorWindow->objectName().isEmpty())
            FileOpenErrorWindow->setObjectName(QString::fromUtf8("FileOpenErrorWindow"));
        FileOpenErrorWindow->resize(241, 352);
        FileOpenErrorWindow->setMinimumSize(QSize(0, 0));
        gridLayout_2 = new QGridLayout(FileOpenErrorWindow);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(FileOpenErrorWindow);
        label->setObjectName(QString::fromUtf8("label"));
        label->setEnabled(true);
        QFont font;
        font.setPointSize(12);
        label->setFont(font);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        textEdit = new QTextEdit(FileOpenErrorWindow);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setEnabled(false);
        textEdit->setMinimumSize(QSize(200, 0));
        textEdit->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);\n"
"color: rgb(0, 0, 0);"));

        gridLayout->addWidget(textEdit, 3, 0, 1, 1);

        label_2 = new QLabel(FileOpenErrorWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setEnabled(true);
        label_2->setFont(font);

        gridLayout->addWidget(label_2, 2, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        retranslateUi(FileOpenErrorWindow);

        QMetaObject::connectSlotsByName(FileOpenErrorWindow);
    } // setupUi

    void retranslateUi(QDialog *FileOpenErrorWindow)
    {
        FileOpenErrorWindow->setWindowTitle(QApplication::translate("FileOpenErrorWindow", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
        label_2->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class FileOpenErrorWindow: public Ui_FileOpenErrorWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILEOPENERRORWINDOW_H
