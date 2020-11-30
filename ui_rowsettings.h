/********************************************************************************
** Form generated from reading UI file 'rowsettings.ui'
**
** Created: Mon 9. Sep 14:40:37 2019
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROWSETTINGS_H
#define UI_ROWSETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_RowSettings
{
public:
    QGridLayout *gridLayout;
    QComboBox *comboBox;
    QComboBox *comboBox_2;
    QLabel *label;
    QLabel *label_2;
    QGroupBox *groupBox;
    QPushButton *Ok;
    QLineEdit *lineEdit;

    void setupUi(QDialog *RowSettings)
    {
        if (RowSettings->objectName().isEmpty())
            RowSettings->setObjectName(QString::fromUtf8("RowSettings"));
        RowSettings->resize(610, 97);
        gridLayout = new QGridLayout(RowSettings);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        comboBox = new QComboBox(RowSettings);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        gridLayout->addWidget(comboBox, 1, 0, 1, 1);

        comboBox_2 = new QComboBox(RowSettings);
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));
        comboBox_2->setMinimumSize(QSize(70, 0));

        gridLayout->addWidget(comboBox_2, 1, 1, 1, 1);

        label = new QLabel(RowSettings);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setFamily(QString::fromUtf8("Times New Roman"));
        font.setPointSize(10);
        label->setFont(font);

        gridLayout->addWidget(label, 0, 2, 1, 1);

        label_2 = new QLabel(RowSettings);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);

        gridLayout->addWidget(label_2, 0, 3, 1, 1);

        groupBox = new QGroupBox(RowSettings);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));

        gridLayout->addWidget(groupBox, 1, 3, 1, 1);

        Ok = new QPushButton(RowSettings);
        Ok->setObjectName(QString::fromUtf8("Ok"));

        gridLayout->addWidget(Ok, 1, 4, 1, 1);

        lineEdit = new QLineEdit(RowSettings);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setMaximumSize(QSize(150, 50));

        gridLayout->addWidget(lineEdit, 1, 2, 1, 1);


        retranslateUi(RowSettings);

        QMetaObject::connectSlotsByName(RowSettings);
    } // setupUi

    void retranslateUi(QDialog *RowSettings)
    {
        RowSettings->setWindowTitle(QApplication::translate("RowSettings", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("RowSettings", "Address", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("RowSettings", "Columns", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        Ok->setText(QApplication::translate("RowSettings", "OK", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RowSettings: public Ui_RowSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROWSETTINGS_H
