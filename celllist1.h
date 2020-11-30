#ifndef CELLLIST1_H
#define CELLLIST1_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTcpSocket>
#include <QComboBox>
#include <QBoxLayout>
#include <QValidator>
#include "ParamStruct.h"
#include "rowsettings.h"
#include "ui_RowSetings.h"
#include "cellvaluechangewindow.h"
#include "dbal.h"
#include "afewrowsettings.h"

const int RowParamStructCounter = 200;
const int ColumnParamStructCounter = 50;


namespace Ui {
    class CellList1;
}

class CellList1 : public QMainWindow
{
    Q_OBJECT

public:
    explicit CellList1(QWidget *parent = 0);
    ~CellList1();
public:
    int CulumnBoxCounter;
    QComboBox *Box;
    QTableWidgetItem *TableItem;
    QBoxLayout *vbox;
    Column_Param_Struct ColumnParalStruct[ColumnParamStructCounter];
    Row_Param_Struct RowParamStruct[RowParamStructCounter];
    Check_Box_Struct CheckBoxStruct[RowParamStructCounter];
    QTcpSocket *CellSocket;
    QTcpSocket *SocketForWrite;
    QByteArray CellSocketData;
    QString SocketData;
    QString RowData[RowParamStructCounter];
    int RowCount;
    QTimer *Timer;
    int NumberOfChannelControl;
    QMenu *ContextMenu;
    Ui::CellList1 *ui;
    QString NameMass[50];
    int CHANNAL;
    int PORT;
    QString IP;
    RowSettings *SettingWindow;
    AFewRowSettings *AFewRowSettingsWindow;
    int ClickedRow;
    int TableTimerInterval;

    void RecombRow_Param_Struct(int Row);

public slots:
    void AddRow();
    void AddColumn();
    void RemoveColumn();
    void RemoveRow();
    void GetParamForColumn(QComboBox *BoxItem);
    void ColumnChangeParam(int);
    void ChangeRowParam();
    void ChangeAFewRowsParam();
    void ReadTableParam();
    void closeEvent(QCloseEvent *e);
    void ConnectAndFormCommand();
    void FormAndSendCommand(int ColumnCounter);
    void ReadInfoFromSocket();
    void InsertInfoIntoCell(int ColumnCounter);
    void DataFilter();
    void StartTimer();
    void ShowContextMenu(const QPoint& pos);
    void ContextMenuChangeRowParam();
    void itemChanged(QTableWidgetItem* ChItem);
    void keyPressEvent(QKeyEvent *event);
    void GetAddressParamList();
    void HaltResumeStateCheck(int CurrentColumn);
    void HaltResStateChRead();
    void AddRowInBack();
    void SaveSelectedCellToBuffer();
    void SetTimerInterval();
    void SetDelay1Sec();
    void SetDelay3Sec();
    void SetDelay5Sec();
};


#endif // CELLLIST1_H
