#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <windows.h>
#include <QString>
#include <QMainWindow>
#include <QTcpSocket>
#include <QDebug>
#include <QDialog>
#include <QWidget>
#include <QtGui>
#include <QTimer>
#include <QTime>
#include <QMenuBar>
#include <QAction>
#include <QFormLayout>
#include <QMessageBox>
#include <math.h>
#include <QKeyEvent>
#include <QLineEdit>
#include "setaddwindow.h"
#include "ParamStruct.h"
#include "ui_fileopenerrorwindow.h"
#include "ReWriteContMenuWidget.h"


extern ParamStruct Quantiti[15];

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTcpSocket* socket; //sock perem
    QByteArray Data; //sock info
    QString DellData;
    QString LocalData;
    int counter;
    int FormatCounter;
    int Kformat;
    QTimer *tmr;
    //IpPort
    QString NameMass[50];
    QAction *AddParam;
    QString IP;
    int PORT;
    int CHANNAL;
    int ControlRepeat;
    int InfoSize;
    int WidthElement;
    int HeightElement;
    int LocalInfoSize;
    int LocalWidthElement;
    int LocalHeightElement;
    float He;
    float Wi;
    int Width;
    int Height;
    long SetAddress;
    int LocalAddress;
    int number;
    int NymberOfChConrolMass[100];
    int NymberOfChennalControl;
    Ui::MainWindow *ui;
    QString Address;
    QMenu *ContextMenu;
    ReWriteContMenuWidget *ReWriteAction;
    int timedelay;



public slots:
    void sockReady(int LocalInfoSize,int LocalWidthEl,int LocalHeightEl);
    void sockDisc();
    void Connect();
    void GetParamToComboBox();
    void ChangeParam(int);
    void on_lineEdit_textEdited(QString);
    void wheelEvent(QWheelEvent *event);


private slots:
    void on_lineEdit_editingFinished();
    void SelectbhwFormat(int);
    void SelectHDBFormat(int);
    void setb();
    void seth();
    void setw();
    void set16bin();
    void set16dec();
    void set16dec2();
    void set16hex();
    void set32bin();
    void set32dec();
    void set32dec2();
    void set32hex();
    void setbin();
    void setdec();
    void setdec2();
    void sethex();
    void Size();
    void Filter();
    //void ResetAddress(int Addr);
    void closeEvent(QCloseEvent *e);
    void AddLineUnderInfo();
    void ShowContextMenu(const QPoint&pos);
    void ContextMenuButtonClicked();

    void SetDelay05Sec();
    void SetDelay1Sec();
    void SetDelay3Sec();
    void SetDelay5Sec();
    void SetTimerInterval();

protected:
    void keyPressEvent(QKeyEvent *event);
    //void mouseDoubleClickEvent(QMouseEvent *event);

private:
    setAddWindow *SetWindow;

};




#endif // MAINWINDOW_H
