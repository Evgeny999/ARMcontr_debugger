#ifndef CELLVALUECHANGEWINDOW_H
#define CELLVALUECHANGEWINDOW_H

#include <QDialog>
#include <QTcpSocket>

namespace Ui {
    class CellValueChangeWindow;
}

class CellValueChangeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CellValueChangeWindow(QWidget *parent = 0);
    ~CellValueChangeWindow();
    QTcpSocket *socket;
    QTimer *tmr;
    int CurrentRow;
    int CurrentColumn;
    int SocketSearchCounter;
    Ui::CellValueChangeWindow *ui;
    CellValueChangeWindow  *CellValueChWindowPointerInClass;
};

#endif // CELLVALUECHANGEWINDOW_H
