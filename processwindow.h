#ifndef PROCESSWINDOW_H
#define PROCESSWINDOW_H

#include <QMainWindow>
#include "mainwindow.h"
#include <QTcpSocket>
#include "dbal.h"


const int ProcessWindowMaxRow=100;
const int BreakPointMax=10;

namespace Ui {
    class ProcessWindow;
}

class ProcessWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProcessWindow(QWidget *parent = 0);
    ~ProcessWindow();
    QTcpSocket *ProcessWindowSocket;
    QByteArray ProcessWindowData;
    QTimer *ProcessWindowTimer;
    QMenu *ContextMenu;
    int NymberOfChConrolMass[100];
    QString NameMass[50];
    QString CurrentPC;
    QString IP;
    int PORT;
    int CHANNEL;
    int NumberOfChannelControl;
    int BreakPoint[BreakPointMax];
    Process_Window_Row_Struct RowInfoStruct[ProcessWindowMaxRow];
    Ui::ProcessWindow *ui;


public slots:
    void GetParamToComboBox();
    void ComboBoxChangeParam(int);
    void closeEvent(QCloseEvent *e);
    void StartWorking();
    void SearchSocket();
    void TakePCaddress();
    void SocketCheckState();
    void ReadFromSocket();
    void ClearUiInfo();
    void ChannelCheck();
    void HaltResumeStateCheck();
    void GetAddressParamList();
    void ReSizeAndAddRow();
    void AddRow();
    void ReadTableParametrs();
    void FormAndSendCommand();
    void FillStructure(int CurrentRow);
    void GetInfoFromCell();
    void SetCurrentAddress();
    void AddBreakPoint();
    void BreakPoitnInfo();
    void RemoveBreakPoint();
    void RemoveAllBreakPoint();
    void MarkBreakPoint();
    void MovePC();
    void MovePC(QString MoveAddress);
    void SetHalt();
    void SetResume();

    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *KeyEvent);
    void ShowContextMenu(const QPoint &pos);

private slots:
    void on_changePCButton_clicked();
    void on_StepButton_clicked();
    void on_StopButton_clicked();
    void on_StartButton_clicked();
};

#endif // PROCESSWINDOW_H
