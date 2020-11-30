/********************************************************************************
** Form generated from reading UI file 'processwindow.ui'
**
** Created: Mon 9. Sep 14:40:37 2019
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROCESSWINDOW_H
#define UI_PROCESSWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProcessWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QTableWidget *tableWidget;
    QLabel *label_2;
    QComboBox *comboBox;
    QLabel *label_4;
    QLabel *label;
    QLineEdit *CurrentAddressLine;
    QLabel *StateLabel;
    QPushButton *StateButton;
    QPushButton *StopButton;
    QPushButton *StartButton;
    QLineEdit *PcLine;
    QRadioButton *TrackPcRadioButton;
    QPushButton *StepButton;
    QLabel *label_3;
    QLineEdit *changePcline;
    QPushButton *changePCButton;
    QStatusBar *statusbar;
    QMenuBar *menubar;

    void setupUi(QMainWindow *ProcessWindow)
    {
        if (ProcessWindow->objectName().isEmpty())
            ProcessWindow->setObjectName(QString::fromUtf8("ProcessWindow"));
        ProcessWindow->resize(479, 520);
        centralwidget = new QWidget(ProcessWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setHorizontalSpacing(0);
        gridLayout_2->setVerticalSpacing(1);
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tableWidget = new QTableWidget(centralwidget);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));

        gridLayout->addWidget(tableWidget, 8, 0, 1, 4);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QFont font;
        font.setPointSize(10);
        label_2->setFont(font);

        gridLayout->addWidget(label_2, 5, 0, 1, 1);

        comboBox = new QComboBox(centralwidget);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        gridLayout->addWidget(comboBox, 0, 0, 1, 3);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font);

        gridLayout->addWidget(label_4, 0, 3, 1, 1);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setFont(font);

        gridLayout->addWidget(label, 2, 0, 1, 1);

        CurrentAddressLine = new QLineEdit(centralwidget);
        CurrentAddressLine->setObjectName(QString::fromUtf8("CurrentAddressLine"));
        CurrentAddressLine->setFont(font);

        gridLayout->addWidget(CurrentAddressLine, 7, 0, 1, 3);

        StateLabel = new QLabel(centralwidget);
        StateLabel->setObjectName(QString::fromUtf8("StateLabel"));
        StateLabel->setFont(font);

        gridLayout->addWidget(StateLabel, 2, 3, 1, 1);

        StateButton = new QPushButton(centralwidget);
        StateButton->setObjectName(QString::fromUtf8("StateButton"));
        StateButton->setEnabled(true);
        StateButton->setMaximumSize(QSize(85, 25));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        StateButton->setFont(font1);

        gridLayout->addWidget(StateButton, 7, 3, 1, 1);

        StopButton = new QPushButton(centralwidget);
        StopButton->setObjectName(QString::fromUtf8("StopButton"));
        StopButton->setMaximumSize(QSize(85, 25));
        StopButton->setFont(font1);

        gridLayout->addWidget(StopButton, 5, 3, 1, 1);

        StartButton = new QPushButton(centralwidget);
        StartButton->setObjectName(QString::fromUtf8("StartButton"));
        StartButton->setMaximumSize(QSize(85, 25));
        StartButton->setFont(font1);

        gridLayout->addWidget(StartButton, 4, 3, 1, 1);

        PcLine = new QLineEdit(centralwidget);
        PcLine->setObjectName(QString::fromUtf8("PcLine"));
        PcLine->setEnabled(false);
        PcLine->setFont(font);
        PcLine->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);\n"
"color: rgb(0, 0, 0);"));

        gridLayout->addWidget(PcLine, 2, 1, 1, 1);

        TrackPcRadioButton = new QRadioButton(centralwidget);
        TrackPcRadioButton->setObjectName(QString::fromUtf8("TrackPcRadioButton"));

        gridLayout->addWidget(TrackPcRadioButton, 2, 2, 1, 1);

        StepButton = new QPushButton(centralwidget);
        StepButton->setObjectName(QString::fromUtf8("StepButton"));
        QFont font2;
        font2.setPointSize(8);
        font2.setBold(true);
        font2.setWeight(75);
        StepButton->setFont(font2);

        gridLayout->addWidget(StepButton, 5, 2, 1, 1);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font);

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        changePcline = new QLineEdit(centralwidget);
        changePcline->setObjectName(QString::fromUtf8("changePcline"));
        changePcline->setFont(font);

        gridLayout->addWidget(changePcline, 4, 1, 1, 1);

        changePCButton = new QPushButton(centralwidget);
        changePCButton->setObjectName(QString::fromUtf8("changePCButton"));
        changePCButton->setFont(font1);

        gridLayout->addWidget(changePCButton, 4, 2, 1, 1);


        gridLayout_2->addLayout(gridLayout, 1, 1, 1, 1);

        ProcessWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(ProcessWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ProcessWindow->setStatusBar(statusbar);
        menubar = new QMenuBar(ProcessWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 479, 20));
        ProcessWindow->setMenuBar(menubar);

        retranslateUi(ProcessWindow);

        QMetaObject::connectSlotsByName(ProcessWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ProcessWindow)
    {
        ProcessWindow->setWindowTitle(QApplication::translate("ProcessWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ProcessWindow", "CurrentAddress:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ProcessWindow", "<-ConnectParam", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ProcessWindow", "Current PC:", 0, QApplication::UnicodeUTF8));
        StateLabel->setText(QApplication::translate("ProcessWindow", "State:Unconnect", 0, QApplication::UnicodeUTF8));
        StateButton->setText(QString());
        StopButton->setText(QApplication::translate("ProcessWindow", "Stop", 0, QApplication::UnicodeUTF8));
        StartButton->setText(QApplication::translate("ProcessWindow", "Start", 0, QApplication::UnicodeUTF8));
        TrackPcRadioButton->setText(QApplication::translate("ProcessWindow", "Track PC", 0, QApplication::UnicodeUTF8));
        StepButton->setText(QApplication::translate("ProcessWindow", "Step", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ProcessWindow", "New PC Value:", 0, QApplication::UnicodeUTF8));
        changePCButton->setText(QApplication::translate("ProcessWindow", "ChangePC", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ProcessWindow: public Ui_ProcessWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROCESSWINDOW_H
