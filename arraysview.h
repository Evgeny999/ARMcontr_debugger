#ifndef ARRAYSVIEW_H
#define ARRAYSVIEW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "mainwindow.h"
#include "varlistelm.h"
#include "dbal.h"

namespace Ui {
    class ArraysView;
}

class ArraysView : public QMainWindow
{
    Q_OBJECT

public:
    explicit ArraysView(QWidget *parent = 0);
    ~ArraysView();

    QTcpSocket *ArrViewSocket;
    QByteArray ArrViewData;
    QTimer *ArrViewTimer;
    QString IPADD;
    int PORT;
    int CHANNEL;
    QString NameMass[50];
    int NymberOfChConrolMass[100];
    int NumberOfChannelControl;
    ReWriteContMenuWidget *RewriteAction;
    QMenu *ContextMenu;

    //InfoParanVar
    int HEXDECBINParam;
    int F8_16_32Param;
    int FormatCounter;
    int Kformat;
    int CurrentAddress;
    int VARSize;
    int Smeschenie;
    int LeftInterval;
    int RightInterval;

    //windowParam
    int Width;
    int Height;
    int WidthElement;
    int HeightElement;
    int InfoSize;

    Ui::ArraysView *ui;

public slots:
    void GetParamToComboBox();
    void ComboBoxChangeParam(int index);
    void StartWorking();
    void SearchSocket();
    void GetFormat();
    void GetSize();
    void SocketCheckState();
    void ReadFromSocket();
    void ChannelCheck();
    void SendCommand();
    void closeEvent(QCloseEvent *);
    void Filter();
    void GetAddressParamList();
    void AddLineUnderInfo();
    void ShowContextMenu(const QPoint &pos);
    void ContextMenuButtonClicked();
};

#endif // ARRAYSVIEW_H
