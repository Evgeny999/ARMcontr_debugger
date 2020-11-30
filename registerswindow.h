#ifndef REGISTERSWINDOW_H
#define REGISTERSWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include "mainwindow.h"

namespace Ui {
    class RegistersWindow;
}

class RegistersWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RegistersWindow(QWidget *parent = 0);
    ~RegistersWindow();
    QTcpSocket *RegWindSocket;
    QTimer *RegWindowTimer;
    QString NameMass[50];
    QByteArray RegWindData;
    int NymberOfChConrolMass[100];
    QString IP;
    int PORT;
    int CHANNEL;
    int NumberOfChannelControl;
    Reg_WindTable_Struct RegInfoStruct[100];
    bool HaltRunState;
    QTableWidgetItem *RegTableItem;
    Ui::RegistersWindow *ui;

public slots:
    void SearchSocket();
    void GetParamToComboBox();
    void ComboBoxChangeParam(int index);
    void StartWorking();
    void SocketCheckState();
    void ChannelCheck();
    void SendCommand();
    void ReadFromSocket();
    void closeEvent(QCloseEvent *e);
    void RegWindowFilter();
    void CheckHaltResumState();
    void InsertInfoIntoTable();


private slots:
    void on_registersTable_itemChanged(QTableWidgetItem* item);
};

#endif // REGISTERSWINDOW_H
