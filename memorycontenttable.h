#ifndef MEMORYCONTENTTABLE_H
#define MEMORYCONTENTTABLE_H

#include <QMainWindow>
#include "mainwindow.h"
#include "varlistelm.h"
#include "dbal.h"

#include "Threads.h"

namespace Ui {
    class MemoryContentTable;
}

class MemoryContentTable : public QMainWindow
{
    Q_OBJECT

public:
    explicit MemoryContentTable(QWidget *parent = 0);
    ~MemoryContentTable();
    Ui::MemoryContentTable *ui;
    QString NameMass[50];
    int NymberOfChConrolMass[100];
    int NumberOfChannelControl;
    QString IPADD;
    int PORT;
    int CHANNEL;
    int AddressLineInfo;

    QTcpSocket *MemoryContentSocket;
    QByteArray MemoryContentData;
    QTimer *MemoryContentTimer;

    int HEXDECBINParam;
    int F8_16_32Param;
    int FormatCounter;
    int Kformat;
    int CurrentAddress;

    int WidthElements;
    int HeightElements;

    void GetParamToComboBox();
    void GetSizes();
    void AddRow();
    void AddColumn();
    void FillTableHeaders();
    void SearchSocket();
    void SockedCheackState();
    void ChannelCheck();
    void SendComand();
    void Filter();
    void FillTable();
    void SetStartTableFormat();
    void wheelEvent(QWheelEvent *event);
    void SetTimerInterval();
    virtual void ChangeInfo();
    int MemoryContentTimerInterval;
    void ReadFromSocket();

public slots:
    void ComboBoxChangeParam(int index);
    void StartWorking();
    void GetFormat();
    void GetAddressParamList();
    void InfoTableItemChanged(QTableWidgetItem* ChItem);
    void closeEvent(QCloseEvent *e);
    void TimerStop();
    void SetDelay05Sec();
    void SetDelay1Sec();
    void SetDelay3Sec();
    void SetDelay5Sec();
    void SaveSelectedCellToBuffer();


};

#endif // MEMORYCONTENTTABLE_H
