#ifndef ALOTOFWINDOW_H
#define ALOTOFWINDOW_H

#include <QMainWindow>
#include <QWorkspace>
#include "fileopenerrorwindow.h"
#include "mainwindow.h"
#include <QCoreApplication>
#include <cstring>
#include "ParamStruct.h"
#include "celllist1.h"
#include "processwindow.h"
#include "registerswindow.h"
#include "arraysview.h"
#include "SpinBoxAction.h"
#include "SaveBeforeCloseAction.h"
#include "memorycontenttable.h"
#include "memorycontenttablecustom.h"

namespace Ui {
    class ALotOfWindow;
}

class ALotOfWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ALotOfWindow(QWidget *parent = 0);
    ~ALotOfWindow();
    QWorkspace *Workspace;
    QString FileInfo;
    FileOpenErrorWindow *ErrorWindow;
    CellList1 *List;
    QMenu *IPPort;
    QAction *IpPort;
    QAction *PortAct;
    QLabel *InfoLabel;
    int control;
    PCStringAction * pcStringAction;
    CellList1 *LastSessionWindow;
    MainWindow *MainWindowLastSession;
    ProcessWindow *ProcWindow;
    RegistersWindow *RegWindow;
    ArraysView *ArrViewWindow;
    SaveBeforeCloseAction *CloseAutSaveAction;
    MemoryContentTable * MemContTable;
    MemoryContentTableCustom *MemContTableCust;

    QAction *AllowAutoSave;
    QAction *BanAutoSave;


private:
    int WindowCounter;
    int SaceCurSessEvent; // 0 - closeEvent ; 1 - userSaveEvent
    bool ExitAutoSave; // 1 Allow 0 nowAllow

private slots:
     void AddWindow();
     void ReadFile();
     void AddCellList();
     void closeEvent(QCloseEvent *e);
     void StartLastSession();
     void AddProccessWindow();
     void AddRegistersWindow();
     void AddArraysViewWindow();
     void SaveCurrentSession();
     void UserSave();
     void UserSaveOpen();
     void ResumeHaltAllProcessors();
     void AllowAutoSaveAction();
     void BanAutoSaveAction();
     void SaveOption();
     void RestoreOption();
     void SetAllProcessPC();
     void CloseAllWindow();
     void AddMemoryContentTableWindow();
     void AddMemoryContentTableCustomWindow();
};


#endif // ALOTOFWINDOW_H
