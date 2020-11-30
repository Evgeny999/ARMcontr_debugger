#ifndef SETADDWINDOW_H
#define SETADDWINDOW_H
#include "ui_setaddwindow.h"
#include <QDialog>
#include <QLineEdit>
#include <QTcpSocket>

namespace Ui {
    class setAddWindow;
}

class setAddWindow : public QDialog
{
    Q_OBJECT

public:
    explicit setAddWindow(QWidget *parent = 0);
    ~setAddWindow();
    Ui::setAddWindow *ui;
    int counter;
    int FormatCounter;
    QTcpSocket *socket;
    QTimer *tmr;

private slots:
    //void on_pushButton_clicked();
    //void closeEvent(QCloseEvent *e);
};

#endif // SETADDWINDOW_H
