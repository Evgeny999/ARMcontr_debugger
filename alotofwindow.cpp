#include "alotofwindow.h"
#include "ui_alotofwindow.h"
#include "ui_fileopenerrorwindow.h"
#include "ui_celllist1.h"
#include "ui_mainwindow.h"
#include "ui_processwindow.h"
#include "ui_registerswindow.h"
#include "ui_arraysview.h"
#include "ui_memorycontenttable.h"


ALotOfWindow::ALotOfWindow(QWidget *parent) :
    QMainWindow(parent)
{
   QTextCodec::setCodecForCStrings(QTextCodec::codecForName("WINDOWS-1251"));
   Workspace = new QWorkspace(this);
   QLayout *lay = Workspace->layout();
   delete lay;
   QGridLayout *layout = new QGridLayout(this);
   Workspace->setLayout(layout);
   InfoLabel = new QLabel(this);
   layout->addWidget(InfoLabel);

   setCentralWidget(Workspace);
   QMenu *file = menuBar()->addMenu("Window");
   QAction *newFile = new QAction(tr("Memory Content"),this);
   newFile->setShortcut(tr("Ctrl+M"));
   connect(newFile,SIGNAL(triggered()),this,SLOT(AddWindow()));
   QAction *CellList = new QAction(tr("Cell List"),this);
   CellList->setShortcut(tr("Ctrl+L"));
   connect(CellList,SIGNAL(triggered()),this,SLOT(AddCellList()));
   QAction *ProcWindow = new QAction(tr("Procesor Window"),this);
   ProcWindow->setShortcut(tr("Ctrl+P"));
   connect(ProcWindow,SIGNAL(triggered()),this,SLOT(AddProccessWindow()));
   QAction *Registers = new QAction(tr("Registers"),this);
   Registers->setShortcut(tr("Ctrl+T"));
   connect(Registers,SIGNAL(triggered()),this,SLOT(AddRegistersWindow()));
   QAction *ArraysView = new QAction(tr("Array View"),this);
   ArraysView->setShortcut(tr("Ctrl+I"));
   connect(ArraysView,SIGNAL(triggered()),this,SLOT(AddArraysViewWindow()));

   QAction *MemoryContentTable = new QAction(tr("Memory Content(Table)"),this);
   connect(MemoryContentTable,SIGNAL(triggered()),this,SLOT(AddMemoryContentTableWindow()));

   QAction *CustMemoryContentTable = new QAction(tr("Custom Memory Content(Table)"),this);
   connect(CustMemoryContentTable,SIGNAL(triggered()),this,SLOT(AddMemoryContentTableCustomWindow()));

   file->addAction(newFile);
   file->addAction(CellList);
   file->addAction(ProcWindow);
   file->addAction(Registers);
   file->addAction(ArraysView);
   file->addAction(MemoryContentTable);
   file->addAction(CustMemoryContentTable);
   file->addSeparator();
   QAction *CloseAllWindowAction = new QAction(tr("Close All Windows"),this);
   connect(CloseAllWindowAction,SIGNAL(triggered()),this,SLOT(CloseAllWindow()));
   file->addAction(CloseAllWindowAction);
   WindowCounter=0;
   QMenu *OpenFile = menuBar()->addMenu("Info");
   IPPort = new QMenu(tr("IP/Port (Info)"), this);
   OpenFile->addMenu(IPPort);

   QMenu *Save = menuBar()->addMenu("Settings");
   QAction *SaveAction = new QAction(tr("Save"),this);
   connect(SaveAction,SIGNAL(triggered()),this,SLOT(UserSave()));
   Save->addAction(SaveAction);
   QAction *OpenSaveAction = new QAction(tr("Open"),this);
   connect(OpenSaveAction,SIGNAL(triggered()),this,SLOT(UserSaveOpen()));
   Save->addAction(OpenSaveAction);
   Save->addSeparator();
   QMenu *AutoSave=Save->addMenu("AutoSave Before Closing");

   CloseAutSaveAction = new SaveBeforeCloseAction();
   connect(CloseAutSaveAction->YesButton,SIGNAL(clicked()),this,SLOT(AllowAutoSaveAction()));
   connect(CloseAutSaveAction->NoButton,SIGNAL(clicked()),this,SLOT(BanAutoSaveAction()));
   AutoSave->addAction(CloseAutSaveAction);
   //AutoSave->addAction()

   QMenu *AllProcessorsMenu = menuBar()->addMenu("ALLProcessors");
   pcStringAction = new PCStringAction();
   AllProcessorsMenu->addAction(pcStringAction);
   pcStringAction->HaltResumeControlVar=1;
   connect(pcStringAction->butt,SIGNAL(clicked()),this,SLOT(SetAllProcessPC()));
   connect(pcStringAction->ResumeHultButton,SIGNAL(clicked()),this,SLOT(ResumeHaltAllProcessors()));

   control= 0;
   ExitAutoSave=true;

  //StartProg
  RestoreOption();
  ALotOfWindow::ReadFile();
  SaceCurSessEvent = 0;
  StartLastSession();
}

ALotOfWindow::~ALotOfWindow()
{
    //delete Quantiti;
    delete Workspace;

}

void ALotOfWindow::AddMemoryContentTableWindow()
{
    MemContTable = new MemoryContentTable(this);
    MemContTable->setWindowTitle("Memory content table");
    Workspace->addWindow(MemContTable);
    MemContTable->show();
}

void ALotOfWindow::AddMemoryContentTableCustomWindow()
{
    MemContTableCust = new MemoryContentTableCustom(this);
    MemContTableCust->setWindowTitle("Custom Memory content table");
    Workspace->addWindow(MemContTableCust);
    MemContTableCust->show();
}

void ALotOfWindow::AddWindow()
{
    WindowCounter++;
    MainWindow *Window = new MainWindow(this);
    Window->setWindowTitle("Debugging Window"+QString::number(WindowCounter));
    Workspace->addWindow(Window);
    Window->show();
}
void ALotOfWindow::AddCellList()
{
    List = new CellList1(this);
    Workspace->addWindow(List);
    List->setWindowTitle("Cell List");
    List->show();
}

void ALotOfWindow::AddProccessWindow()
{
    ProcWindow = new ProcessWindow(this);
    ProcWindow->setWindowTitle("Processor Window");
    Workspace->addWindow(ProcWindow);
    ProcWindow->show();
}

void ALotOfWindow::AddRegistersWindow()
{
    RegWindow  = new RegistersWindow(this);
    RegWindow->setWindowTitle("Registers");
    Workspace->addWindow(RegWindow);
    RegWindow->show();
}

void ALotOfWindow::AddArraysViewWindow()
{
    ArrViewWindow = new ArraysView(this);
    ArrViewWindow->setWindowTitle("Arrays View Window");
    Workspace->addWindow(ArrViewWindow);
    ArrViewWindow->show();
}

void ALotOfWindow::closeEvent(QCloseEvent *e)
{
    SaceCurSessEvent = 0;
    if(ExitAutoSave==true)
    {
       SaveCurrentSession();
    }
    else
    {
        QFile File("Last.txt");
        File.open(QIODevice::WriteOnly | QIODevice::Truncate);
        File.close();
    }
    SaveOption();
}
void ALotOfWindow::UserSave()
{
    SaceCurSessEvent = 1;
    SaveCurrentSession();
}
void ALotOfWindow::UserSaveOpen()
{
    SaceCurSessEvent = 1;
    StartLastSession();
}

void ALotOfWindow::SaveCurrentSession()
{
    QFile File;
    if(SaceCurSessEvent==0)
    {
        File.setName("Last.txt");
    }
    if(SaceCurSessEvent==1)
    {
        File.setName("Save.txt");
    }

    if(File.open(QIODevice::WriteOnly))
   {
    for(int i=0; i<Workspace->windowList().size();i++)
    {
         if(strcmp(Workspace->windowList().at(i)->metaObject()->className(),"ArraysView")==0)
         {
              ArraysView *ArrViewWindow=static_cast<ArraysView*>(Workspace->windowList().at(i));
              File.write("{ArraysView ");
              File.write(QString("x:")+QString::number(ArrViewWindow->parentWidget()->x())+QString(' '));
              File.write(QString("y:")+QString::number(ArrViewWindow->parentWidget()->y())+QString(' '));
              File.write(QString("width:")+QString::number(ArrViewWindow->parentWidget()->width())+QString(' '));
              File.write(QString("height:")+QString::number(ArrViewWindow->parentWidget()->height())+QString(' '));
              File.write(QString("Address:")+QString(ArrViewWindow->ui->AddrLine->text())+QString(' '));
              File.write(QString("IPC:")+QString::number(ArrViewWindow->ui->SettingComboBox->currentIndex())+QString(' '));
              File.write(QString("RAZ:")+QString::number(ArrViewWindow->ui->RazrComboBox->currentIndex())+QString(' '));
              File.write(QString("HDB:")+QString::number(ArrViewWindow->ui->VIdComboBox->currentIndex())+QString(' '));
              File.write(QString("Par:")+QString(ArrViewWindow->ui->SizeLine->text()+' '+ArrViewWindow->ui->ScmechenieLine->text()+' '+ArrViewWindow->ui->IntervalLine->text())+QString(' '));
              File.write(QString("}"));
         }

         if(strcmp(Workspace->windowList().at(i)->metaObject()->className(),"MainWindow")==0)
         {
              MainWindow *wind=static_cast<MainWindow*>(Workspace->windowList().at(i));
              File.write("{MainWindow ");
              File.write(QString("x:")+QString::number(wind->parentWidget()->x())+QString(' '));
              File.write(QString("y:")+QString::number(wind->parentWidget()->y())+QString(' '));
              File.write(QString("width:")+QString::number(wind->parentWidget()->width())+QString(' '));
              File.write(QString("height:")+QString::number(wind->parentWidget()->height())+QString(' '));
              File.write(QString().sprintf("Address:%X ",wind->SetAddress));
              File.write(QString("IPC:")+QString::number(wind->ui->comboBox_3->currentIndex())+QString(' '));
              File.write(QString("RAZ:")+QString::number(wind->ui->comboBox->currentIndex())+QString(' '));
              File.write(QString("HDB:")+QString::number(wind->ui->comboBox_2->currentIndex())+QString(' '));
              File.write(QString("}"));
         }

         if(strcmp(Workspace->windowList().at(i)->metaObject()->className(),"MemoryContentTable")==0)
         {
              MemoryContentTable *MemContTab=static_cast<MemoryContentTable*>(Workspace->windowList().at(i));
              File.write("{MemoryTable ");
              File.write(QString("x:")+QString::number(MemContTab->parentWidget()->x())+QString(' '));
              File.write(QString("y:")+QString::number(MemContTab->parentWidget()->y())+QString(' '));
              File.write(QString("width:")+QString::number(MemContTab->parentWidget()->width())+QString(' '));
              File.write(QString("height:")+QString::number(MemContTab->parentWidget()->height())+QString(' '));
              File.write(QString("Address:"+MemContTab->ui->AddressLine->text()+" "));
              File.write(QString("IPC:")+QString::number(MemContTab->ui->SettingsComboBox->currentIndex())+QString(' '));
              File.write(QString("RAZ:")+QString::number(MemContTab->ui->FormatComboBox->currentIndex())+QString(' '));
              File.write(QString("HDB:")+QString::number(MemContTab->ui->HexDecBinComboBox->currentIndex())+QString(' '));
              File.write(QString("}"));
         }

         if(strcmp(Workspace->windowList().at(i)->metaObject()->className(),"ProcessWindow")==0)
         {
              ProcessWindow *ProcWind=static_cast<ProcessWindow*>(Workspace->windowList().at(i));
              File.write("{ProcessWindow ");
              File.write(QString("x:")+QString::number(ProcWind->parentWidget()->x())+QString(' '));
              File.write(QString("y:")+QString::number(ProcWind->parentWidget()->y())+QString(' '));
              File.write(QString("width:")+QString::number(ProcWind->parentWidget()->width())+QString(' '));
              File.write(QString("height:")+QString::number(ProcWind->parentWidget()->height())+QString(' '));
              File.write(QString("CBOX:")+QString::number(ProcWind->ui->comboBox->currentIndex())+QString(' '));
              File.write(QString("CURA:")+QString (ProcWind->ui->CurrentAddressLine->text())+QString(' '));
              File.write(QString("}"));
         }

         if(strcmp(Workspace->windowList().at(i)->metaObject()->className(),"RegistersWindow")==0)
         {
              RegistersWindow *RegWind=static_cast<RegistersWindow*>(Workspace->windowList().at(i));
              File.write("{RegistersWindow ");
              File.write(QString("x:")+QString::number(RegWind->parentWidget()->x())+QString(' '));
              File.write(QString("y:")+QString::number(RegWind->parentWidget()->y())+QString(' '));
              File.write(QString("width:")+QString::number(RegWind->parentWidget()->width())+QString(' '));
              File.write(QString("height:")+QString::number(RegWind->parentWidget()->height())+QString(' '));
              File.write(QString("CBOX:")+QString::number(RegWind->ui->comboBox->currentIndex())+QString(' '));
              File.write(QString("}"));
         }

         if(strcmp(Workspace->windowList().at(i)->metaObject()->className(),"CellList1")==0)
         {
             CellList1 *List=static_cast<CellList1*>(Workspace->windowList().at(i));
             File.write("{CellList ");
             File.write(QString("x:")+QString::number(List->parentWidget()->x())+QString(' '));
             File.write(QString("y:")+QString::number(List->parentWidget()->y())+QString(' '));
             File.write(QString("width:")+QString::number(List->parentWidget()->width())+QString(' '));
             File.write(QString("height:")+QString::number(List->parentWidget()->height())+QString(' '));
             File.write(QString("rowC:")+QString::number(List->ui->Table->rowCount())+QString(' '));
             File.write(QString("colC:")+QString::number(List->ui->Table->columnCount())+QString(' '));
             File.write(QString("CBI:("));//Combo  BOx Index
             for(int i=1;i<List->ui->Table->columnCount();i++)
             {
                 int ComboBoxCurrentIndex;
                 QComboBox *CurrentComboBox= qobject_cast<QComboBox*>(List->ui->Table->cellWidget(0,i));
                 ComboBoxCurrentIndex=CurrentComboBox->currentIndex();
                 File.write(QString::number(ComboBoxCurrentIndex)+QString(" "));

             }
             File.write(QString(") "));
             File.write(QString("ADR:("));//Combo  BOx Index
             for(int i=1;i<List->ui->Table->rowCount();i++)
             {
                 QLineEdit *CurrentLine=qobject_cast<QLineEdit*>(List->ui->Table->cellWidget(i,0));
                 File.write(QString(CurrentLine->text())+QString(" "));
             }
             File.write(QString(") "));
             File.write(QString("LiIn:")+"\r\n");
             /*
                typedef struct{
                    QString Address;
                    int HexDecBinCount;
                    int Count8_16_32;
                    bool ExitFlag;
                }Row_Param_Struct;
              */
             for(int i=0;i<RowParamStructCounter;i++)
             {
                 File.write(QString::number(List->RowParamStruct[i].Count8_16_32)+" ");
                 File.write(QString::number(List->RowParamStruct[i].HexDecBinCount)+"\r\n");
             }
             File.write(QString("}"));
         }
    }
  }
    File.close();
}

void ALotOfWindow::StartLastSession()
{
    //InfoParam
    int Param8_16_32=0;
    int Param_hex_dec_bin=0;
    int rowCount=0;
    int columnCount=0;
    int ColumnComboBoxParamMass[ColumnParamStructCounter];
    QString RowAddressMass[RowParamStructCounter];
    int CurrentColumn;
    //ConnectParam
    QString FileInfo;
    int CurrentAdress=0;
    int IP_PORT_CHANNAL_parametr=0;
    //Geometr param
    bool ok;
    int x=0;
    int y=0;
    int width=0;
    int height=0;
    int FileInfoCounter = 0;
    QFile File;
    if(SaceCurSessEvent==0)
    {
        File.setName("Last.txt");
    }
    if(SaceCurSessEvent==1)
    {
        File.setName("Save.txt");
    }
    if(File.open(QIODevice::ReadOnly))
    {
       QByteArray ByteFileInfo=File.readAll();
       if(File.size()==0) QMessageBox::warning(this,"Warning","Last ssesion not found");
       File.close();
       FileInfo=QString(ByteFileInfo);
       QString WindowParamInfo;
       QString CurrAddr;
       while(FileInfo[FileInfoCounter]!='\0')
       {
           //Search CellLisWindwo
           if((FileInfo[FileInfoCounter]=='{')&&(FileInfo[FileInfoCounter+1]=='C')&&(FileInfo[FileInfoCounter+2]=='e')&&(FileInfo[FileInfoCounter+3]=='l')&&(FileInfo[FileInfoCounter+4]=='l'))
           {
               LastSessionWindow = new CellList1(this);
               while((FileInfo[FileInfoCounter]!='}'))
               {
                  if((FileInfo[FileInfoCounter]=='x')&&(FileInfo[FileInfoCounter+1]==':'))
                   {
                      FileInfoCounter=FileInfoCounter+2;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      x=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                   }
                  if((FileInfo[FileInfoCounter]=='y')&&(FileInfo[FileInfoCounter+1]==':'))
                   {
                      FileInfoCounter=FileInfoCounter+2;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      y=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                   }
                   if((FileInfo[FileInfoCounter]=='w')&&(FileInfo[FileInfoCounter+1]=='i')&&(FileInfo[FileInfoCounter+2]=='d'))//width:
                   {
                      FileInfoCounter=FileInfoCounter+6;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      width=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                    }
                   if((FileInfo[FileInfoCounter]=='h')&&(FileInfo[FileInfoCounter+1]=='e')&&(FileInfo[FileInfoCounter+2]=='i'))//height:
                   {
                      FileInfoCounter=FileInfoCounter+7;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      height=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                    }
                    if((FileInfo[FileInfoCounter]=='r')&&(FileInfo[FileInfoCounter+1]=='o')&&(FileInfo[FileInfoCounter+2]=='w')&&(FileInfo[FileInfoCounter+3]=='C'))//rowC:
                     {
                        FileInfoCounter=FileInfoCounter+5;
                        while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                        rowCount=WindowParamInfo.toInt();
                        WindowParamInfo.clear();
                      }
                     if((FileInfo[FileInfoCounter]=='c')&&(FileInfo[FileInfoCounter+1]=='o')&&(FileInfo[FileInfoCounter+2]=='l')&&(FileInfo[FileInfoCounter+3]=='C'))//colC:
                     {
                        FileInfoCounter=FileInfoCounter+5;
                        while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                        columnCount=WindowParamInfo.toInt();
                        WindowParamInfo.clear();
                     }

                     if((FileInfo[FileInfoCounter]=='C')&&(FileInfo[FileInfoCounter+1]=='B')&&(FileInfo[FileInfoCounter+2]=='I'))//:
                     {
                         int ColumnMassCounter=0;
                        FileInfoCounter=FileInfoCounter+4;
                        if(FileInfo[FileInfoCounter]=='(')
                        {
                            FileInfoCounter=FileInfoCounter+1;
                            while(FileInfo[FileInfoCounter]!=')')
                            {
                                if(FileInfo[FileInfoCounter]!=' ')
                                {
                                    while(FileInfo[FileInfoCounter]!=' ')
                                    {
                                        WindowParamInfo.append(FileInfo[FileInfoCounter]);
                                        FileInfoCounter++;
                                    }
                                    CurrentColumn=WindowParamInfo.toInt();
                                    ColumnComboBoxParamMass[ColumnMassCounter]=CurrentColumn;
                                    ColumnMassCounter++;
                                    WindowParamInfo.clear();
                                }
                                FileInfoCounter++;
                            }
                        }
                        WindowParamInfo.clear();
                     }
                     if((FileInfo[FileInfoCounter]=='A')&&(FileInfo[FileInfoCounter+1]=='D')&&(FileInfo[FileInfoCounter+2]=='R'))//RowAddress:
                     {
                        int RowAddressMassCount=0;
                        FileInfoCounter=FileInfoCounter+4;
                        if(FileInfo[FileInfoCounter]=='(')
                        {
                            FileInfoCounter=FileInfoCounter+1;
                            while(FileInfo[FileInfoCounter]!=')')
                            {
                                if(FileInfo[FileInfoCounter]!=' ')
                                {

                                    while(FileInfo[FileInfoCounter]!=' ')
                                    {
                                        RowAddressMass[RowAddressMassCount].append(FileInfo[FileInfoCounter]);
                                        FileInfoCounter++;
                                    }
                                    RowAddressMassCount++;
                                }
                                FileInfoCounter++;
                            }
                        }
                        WindowParamInfo.clear();
                     }

                   if((FileInfo[FileInfoCounter]=='L')&&(FileInfo[FileInfoCounter+1]=='i')&&(FileInfo[FileInfoCounter+2]=='I'))
                   {
                       QString P8_16_32_Format;
                       QString Phex_dec_bin_Format;
                       bool ReadControl=0;
                       int InfoIterator=0;

                       FileInfoCounter=FileInfoCounter+6;
                       for(int i=0;i<RowParamStructCounter;i++)
                       {
                           FileInfoCounter++;
                           P8_16_32_Format.clear();
                           Phex_dec_bin_Format.clear();
                           ReadControl=0;
                           InfoIterator=0;
                           while(FileInfo[FileInfoCounter]!='\n')
                           {
                               if(FileInfo[FileInfoCounter]==' ') {ReadControl=1; InfoIterator=0;}
                               if(ReadControl==0)
                               {
                                   P8_16_32_Format[InfoIterator]=FileInfo[FileInfoCounter];
                               }
                               else
                               {
                                   Phex_dec_bin_Format[InfoIterator]=FileInfo[FileInfoCounter];
                               }
                               InfoIterator++;
                               FileInfoCounter++;
                           }
                           LastSessionWindow->RowParamStruct[i].Count8_16_32=P8_16_32_Format.toInt();
                           LastSessionWindow->RowParamStruct[i].HexDecBinCount=Phex_dec_bin_Format.toInt();
                       }
                   }


                  FileInfoCounter++;
               }
               for(int RRow=0;RRow<rowCount-2;RRow++){ LastSessionWindow->AddRowInBack();}
               for(int CCol=0;CCol<columnCount-2;CCol++) {LastSessionWindow->AddColumn();}
               for(int ComboBoxCont=1;ComboBoxCont<LastSessionWindow->ui->Table->columnCount();ComboBoxCont++)
               {
                   QComboBox *CurrentComboBox = qobject_cast<QComboBox*>(LastSessionWindow->ui->Table->cellWidget(0,ComboBoxCont));
                   CurrentComboBox->setCurrentIndex(ColumnComboBoxParamMass[ComboBoxCont-1]);
               }
               for(int RowAddressCount=1;RowAddressCount<LastSessionWindow->ui->Table->rowCount();RowAddressCount++)
               {
                   //LastSessionWindow->ui->Table->item(RowAddressCount,0)->setText(RowAddressMass[RowAddressCount-1]);
                   QLineEdit* LineEdit=qobject_cast<QLineEdit*>(LastSessionWindow->ui->Table->cellWidget(RowAddressCount,0));
                   LineEdit->setText(RowAddressMass[RowAddressCount-1]);
               }
               LastSessionWindow->setGeometry(x,y,width,height);
               LastSessionWindow->setWindowTitle("CellList");
               Workspace->addWindow(LastSessionWindow)->show();
               for(int i =0;i<RowParamStructCounter ;i++) RowAddressMass[i].clear();
           }
           //Searc ProcessWindow
           if((FileInfo[FileInfoCounter]=='{')&&(FileInfo[FileInfoCounter+1]=='P')&&(FileInfo[FileInfoCounter+2]=='r')&&(FileInfo[FileInfoCounter+3]=='o')&&(FileInfo[FileInfoCounter+4]=='c'))
           {
               ProcWindow = new ProcessWindow(this);
               while((FileInfo[FileInfoCounter]!='}'))
               {
                   if((FileInfo[FileInfoCounter]=='x')&&(FileInfo[FileInfoCounter+1]==':'))
                        {
                           FileInfoCounter=FileInfoCounter+2;
                           while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                           x=WindowParamInfo.toInt();
                           WindowParamInfo.clear();
                        }
                   if((FileInfo[FileInfoCounter]=='y')&&(FileInfo[FileInfoCounter+1]==':'))
                        {
                           FileInfoCounter=FileInfoCounter+2;
                           while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                           y=WindowParamInfo.toInt();
                           WindowParamInfo.clear();
                        }
                   if((FileInfo[FileInfoCounter]=='w')&&(FileInfo[FileInfoCounter+1]=='i')&&(FileInfo[FileInfoCounter+2]=='d'))//width:
                       {
                          FileInfoCounter=FileInfoCounter+6;
                          while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                          width=WindowParamInfo.toInt();
                          WindowParamInfo.clear();
                        }
                   if((FileInfo[FileInfoCounter]=='h')&&(FileInfo[FileInfoCounter+1]=='e')&&(FileInfo[FileInfoCounter+2]=='i'))//height:
                       {
                          FileInfoCounter=FileInfoCounter+7;
                          while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                          height=WindowParamInfo.toInt();
                          WindowParamInfo.clear();
                       }
                   if((FileInfo[FileInfoCounter]=='C')&&(FileInfo[FileInfoCounter+1]=='B')&&(FileInfo[FileInfoCounter+2]=='O'))//Ip Port channal:
                       {
                         FileInfoCounter=FileInfoCounter+5;
                         while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                         IP_PORT_CHANNAL_parametr=WindowParamInfo.toInt();
                         WindowParamInfo.clear();
                       }
                   if((FileInfo[FileInfoCounter]=='C')&&(FileInfo[FileInfoCounter+1]=='U')&&(FileInfo[FileInfoCounter+2]=='R'))
                       {
                         CurrAddr.clear();
                         FileInfoCounter=FileInfoCounter+5;
                         while(FileInfo[FileInfoCounter]!=' ') {CurrAddr.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                         WindowParamInfo.clear();
                       }
                   FileInfoCounter++;
               }
               ProcWindow->setGeometry(x,y,width,height);
               ProcWindow->ui->comboBox->setCurrentIndex(IP_PORT_CHANNAL_parametr);
               ProcWindow->ui->CurrentAddressLine->setText(CurrAddr);
               Workspace->addWindow(ProcWindow);
               ProcWindow->show();
           }
           //RegWindow
           if((FileInfo[FileInfoCounter]=='{')&&(FileInfo[FileInfoCounter+1]=='R')&&(FileInfo[FileInfoCounter+2]=='e')&&(FileInfo[FileInfoCounter+3]=='g')&&(FileInfo[FileInfoCounter+4]=='i'))
           {
               RegWindow = new RegistersWindow(this);
               while((FileInfo[FileInfoCounter]!='}'))
               {
                   if((FileInfo[FileInfoCounter]=='x')&&(FileInfo[FileInfoCounter+1]==':'))
                        {
                           FileInfoCounter=FileInfoCounter+2;
                           while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                           x=WindowParamInfo.toInt();
                           WindowParamInfo.clear();
                        }
                   if((FileInfo[FileInfoCounter]=='y')&&(FileInfo[FileInfoCounter+1]==':'))
                        {
                           FileInfoCounter=FileInfoCounter+2;
                           while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                           y=WindowParamInfo.toInt();
                           WindowParamInfo.clear();
                        }
                   if((FileInfo[FileInfoCounter]=='w')&&(FileInfo[FileInfoCounter+1]=='i')&&(FileInfo[FileInfoCounter+2]=='d'))//width:
                       {
                          FileInfoCounter=FileInfoCounter+6;
                          while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                          width=WindowParamInfo.toInt();
                          WindowParamInfo.clear();
                        }
                   if((FileInfo[FileInfoCounter]=='h')&&(FileInfo[FileInfoCounter+1]=='e')&&(FileInfo[FileInfoCounter+2]=='i'))//height:
                       {
                          FileInfoCounter=FileInfoCounter+7;
                          while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                          height=WindowParamInfo.toInt();
                          WindowParamInfo.clear();
                       }
                   if((FileInfo[FileInfoCounter]=='C')&&(FileInfo[FileInfoCounter+1]=='B')&&(FileInfo[FileInfoCounter+2]=='O'))//Ip Port channal:
                       {
                         FileInfoCounter=FileInfoCounter+5;
                         while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                         IP_PORT_CHANNAL_parametr=WindowParamInfo.toInt();
                         WindowParamInfo.clear();
                       }
                   FileInfoCounter++;
               }
               RegWindow->setGeometry(x,y,width,height);
               RegWindow->ui->comboBox->setCurrentIndex(IP_PORT_CHANNAL_parametr);
               Workspace->addWindow(RegWindow);
               RegWindow->show();
           }
           //Search MainWindow window
           if((FileInfo[FileInfoCounter]=='{')&&(FileInfo[FileInfoCounter+1]=='M')&&(FileInfo[FileInfoCounter+2]=='a')&&(FileInfo[FileInfoCounter+3]=='i')&&(FileInfo[FileInfoCounter+4]=='n'))
           {
               MainWindowLastSession = new MainWindow(this);
               while((FileInfo[FileInfoCounter]!='}'))
               {
                   if((FileInfo[FileInfoCounter]=='x')&&(FileInfo[FileInfoCounter+1]==':'))
                    {
                       FileInfoCounter=FileInfoCounter+2;
                       while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                       x=WindowParamInfo.toInt();
                       WindowParamInfo.clear();
                    }
                   if((FileInfo[FileInfoCounter]=='y')&&(FileInfo[FileInfoCounter+1]==':'))
                    {
                       FileInfoCounter=FileInfoCounter+2;
                       while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                       y=WindowParamInfo.toInt();
                       WindowParamInfo.clear();
                    }
                   if((FileInfo[FileInfoCounter]=='w')&&(FileInfo[FileInfoCounter+1]=='i')&&(FileInfo[FileInfoCounter+2]=='d'))//width:
                   {
                      FileInfoCounter=FileInfoCounter+6;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      width=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                   }
                   if((FileInfo[FileInfoCounter]=='h')&&(FileInfo[FileInfoCounter+1]=='e')&&(FileInfo[FileInfoCounter+2]=='i'))//height:
                   {
                      FileInfoCounter=FileInfoCounter+7;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      height=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                   }
                   if((FileInfo[FileInfoCounter]=='A')&&(FileInfo[FileInfoCounter+1]=='d')&&(FileInfo[FileInfoCounter+2]=='d'))//CurrentAddress:
                   {
                      FileInfoCounter=FileInfoCounter+8;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      CurrentAdress=WindowParamInfo.toInt(&ok,16);
                      WindowParamInfo.clear();
                   }
                   if((FileInfo[FileInfoCounter]=='R')&&(FileInfo[FileInfoCounter+1]=='A')&&(FileInfo[FileInfoCounter+2]=='Z'))//CurrentAddress:
                   {
                      FileInfoCounter=FileInfoCounter+4;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      Param8_16_32=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                   }
                   if((FileInfo[FileInfoCounter]=='H')&&(FileInfo[FileInfoCounter+1]=='D')&&(FileInfo[FileInfoCounter+2]=='B'))//CurrentAddress:
                   {
                      FileInfoCounter=FileInfoCounter+4;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      Param_hex_dec_bin=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                    }
                    if((FileInfo[FileInfoCounter]=='I')&&(FileInfo[FileInfoCounter+1]=='P')&&(FileInfo[FileInfoCounter+2]=='C'))//Ip Port channal:
                    {
                      FileInfoCounter=FileInfoCounter+4;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      IP_PORT_CHANNAL_parametr=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                    }
                    FileInfoCounter++;
               }
               MainWindowLastSession->ui->lineEdit->setText(QString().sprintf("0x%08X",CurrentAdress));
               MainWindowLastSession->ui->comboBox_3->setCurrentIndex(IP_PORT_CHANNAL_parametr);
               MainWindowLastSession->ui->comboBox->setCurrentIndex(Param8_16_32);
               MainWindowLastSession->ui->comboBox_2->setCurrentIndex(Param_hex_dec_bin);
               MainWindowLastSession->setGeometry(x,y,width,height);
               MainWindowLastSession->setWindowTitle("DebuggingWindow");
               Workspace->addWindow(MainWindowLastSession);
               MainWindowLastSession->show();
           }


           //Content table window
           if((FileInfo[FileInfoCounter]=='{')&&(FileInfo[FileInfoCounter+1]=='M')&&(FileInfo[FileInfoCounter+2]=='e')&&(FileInfo[FileInfoCounter+3]=='m')&&(FileInfo[FileInfoCounter+4]=='o'))
           {
               QString ContentTableAddresString;
               MemContTable = new MemoryContentTable(this);
               while((FileInfo[FileInfoCounter]!='}'))
               {
                   if((FileInfo[FileInfoCounter]=='x')&&(FileInfo[FileInfoCounter+1]==':'))
                    {
                       FileInfoCounter=FileInfoCounter+2;
                       while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                       x=WindowParamInfo.toInt();
                       WindowParamInfo.clear();
                    }
                   if((FileInfo[FileInfoCounter]=='y')&&(FileInfo[FileInfoCounter+1]==':'))
                    {
                       FileInfoCounter=FileInfoCounter+2;
                       while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                       y=WindowParamInfo.toInt();
                       WindowParamInfo.clear();
                    }
                   if((FileInfo[FileInfoCounter]=='w')&&(FileInfo[FileInfoCounter+1]=='i')&&(FileInfo[FileInfoCounter+2]=='d'))//width:
                   {
                      FileInfoCounter=FileInfoCounter+6;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      width=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                   }
                   if((FileInfo[FileInfoCounter]=='h')&&(FileInfo[FileInfoCounter+1]=='e')&&(FileInfo[FileInfoCounter+2]=='i'))//height:
                   {
                      FileInfoCounter=FileInfoCounter+7;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      height=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                   }
                   if((FileInfo[FileInfoCounter]=='A')&&(FileInfo[FileInfoCounter+1]=='d')&&(FileInfo[FileInfoCounter+2]=='d'))//CurrentAddress:
                   {
                      FileInfoCounter=FileInfoCounter+8;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      CurrentAdress=WindowParamInfo.toInt(&ok,16);
                      ContentTableAddresString=WindowParamInfo;
                      WindowParamInfo.clear();
                   }
                   if((FileInfo[FileInfoCounter]=='R')&&(FileInfo[FileInfoCounter+1]=='A')&&(FileInfo[FileInfoCounter+2]=='Z'))//CurrentAddress:
                   {
                      FileInfoCounter=FileInfoCounter+4;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      Param8_16_32=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                   }
                   if((FileInfo[FileInfoCounter]=='H')&&(FileInfo[FileInfoCounter+1]=='D')&&(FileInfo[FileInfoCounter+2]=='B'))//CurrentAddress:
                   {
                      FileInfoCounter=FileInfoCounter+4;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      Param_hex_dec_bin=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                    }
                    if((FileInfo[FileInfoCounter]=='I')&&(FileInfo[FileInfoCounter+1]=='P')&&(FileInfo[FileInfoCounter+2]=='C'))//Ip Port channal:
                    {
                      FileInfoCounter=FileInfoCounter+4;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      IP_PORT_CHANNAL_parametr=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                    }
                    FileInfoCounter++;
               }
               MemContTable->ui->AddressLine->setText(ContentTableAddresString);
               MemContTable->ui->SettingsComboBox->setCurrentIndex(IP_PORT_CHANNAL_parametr);
               MemContTable->ui->FormatComboBox->setCurrentIndex(Param8_16_32);
               MemContTable->ui->HexDecBinComboBox->setCurrentIndex(Param_hex_dec_bin);
               MemContTable->setGeometry(x,y,width,height);
               MemContTable->setWindowTitle("Memory content(Table)");
               Workspace->addWindow(MemContTable);
               MemContTable->show();
           }

           //ArrViewWindow
           if((FileInfo[FileInfoCounter]=='{')&&(FileInfo[FileInfoCounter+1]=='A')&&(FileInfo[FileInfoCounter+2]=='r')&&(FileInfo[FileInfoCounter+3]=='r')&&(FileInfo[FileInfoCounter+4]=='a'))
           {
               QString Address;
               QString DopParametrArrView;
               QString SetDopParamString;
               ArrViewWindow = new ArraysView(this);
               while((FileInfo[FileInfoCounter]!='}'))
               {
                   if((FileInfo[FileInfoCounter]=='x')&&(FileInfo[FileInfoCounter+1]==':'))
                    {
                       FileInfoCounter=FileInfoCounter+2;
                       while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                       x=WindowParamInfo.toInt();
                       WindowParamInfo.clear();
                    }
                   if((FileInfo[FileInfoCounter]=='y')&&(FileInfo[FileInfoCounter+1]==':'))
                    {
                       FileInfoCounter=FileInfoCounter+2;
                       while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                       y=WindowParamInfo.toInt();
                       WindowParamInfo.clear();
                    }
                   if((FileInfo[FileInfoCounter]=='w')&&(FileInfo[FileInfoCounter+1]=='i')&&(FileInfo[FileInfoCounter+2]=='d'))//width:
                   {
                      FileInfoCounter=FileInfoCounter+6;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      width=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                   }
                   if((FileInfo[FileInfoCounter]=='h')&&(FileInfo[FileInfoCounter+1]=='e')&&(FileInfo[FileInfoCounter+2]=='i'))//height:
                   {
                      FileInfoCounter=FileInfoCounter+7;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      height=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                   }
                   if((FileInfo[FileInfoCounter]=='A')&&(FileInfo[FileInfoCounter+1]=='d')&&(FileInfo[FileInfoCounter+2]=='d'))//CurrentAddress:
                   {
                      FileInfoCounter=FileInfoCounter+8;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      Address=WindowParamInfo;
                      WindowParamInfo.clear();
                   }
                   if((FileInfo[FileInfoCounter]=='R')&&(FileInfo[FileInfoCounter+1]=='A')&&(FileInfo[FileInfoCounter+2]=='Z'))//CurrentAddress:
                   {
                      FileInfoCounter=FileInfoCounter+4;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      Param8_16_32=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                   }
                   if((FileInfo[FileInfoCounter]=='H')&&(FileInfo[FileInfoCounter+1]=='D')&&(FileInfo[FileInfoCounter+2]=='B'))//CurrentAddress:
                   {
                      FileInfoCounter=FileInfoCounter+4;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      Param_hex_dec_bin=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                    }
                    if((FileInfo[FileInfoCounter]=='I')&&(FileInfo[FileInfoCounter+1]=='P')&&(FileInfo[FileInfoCounter+2]=='C'))//Ip Port channal:
                    {
                      FileInfoCounter=FileInfoCounter+4;
                      while(FileInfo[FileInfoCounter]!=' ') {WindowParamInfo.append(FileInfo[FileInfoCounter]); FileInfoCounter++;}
                      IP_PORT_CHANNAL_parametr=WindowParamInfo.toInt();
                      WindowParamInfo.clear();
                    }
                    if((FileInfo[FileInfoCounter]=='P')&&(FileInfo[FileInfoCounter+1]=='a')&&(FileInfo[FileInfoCounter+2]=='r'))//Ip Port channal:
                    {
                      int ProbelControl=0;
                      FileInfoCounter=FileInfoCounter+4;
                      while(ProbelControl!=3) {DopParametrArrView.append(FileInfo[FileInfoCounter]); FileInfoCounter++;if(FileInfo[FileInfoCounter]==' '){ProbelControl++;}}
                    }
                    FileInfoCounter++;
               }
               SetDopParamString.clear();
               int iterator=0;
               while(DopParametrArrView[iterator]!='\0')
               {
                   while(DopParametrArrView[iterator]!=' '){SetDopParamString.append(DopParametrArrView[iterator]);iterator++;}
                   ArrViewWindow->ui->SizeLine->setText(SetDopParamString);
                   SetDopParamString.clear();
                   iterator++;
                   while(DopParametrArrView[iterator]!=' '){SetDopParamString.append(DopParametrArrView[iterator]);iterator++;}
                   ArrViewWindow->ui->ScmechenieLine->setText(SetDopParamString);
                   SetDopParamString.clear();
                   iterator++;
                   while(DopParametrArrView[iterator]!='\0'){SetDopParamString.append(DopParametrArrView[iterator]);iterator++;}
                   ArrViewWindow->ui->IntervalLine->setText(SetDopParamString);
                   SetDopParamString.clear();
                   if(DopParametrArrView[iterator]!='\0') iterator++;
               }
               ArrViewWindow->ui->AddrLine->setText(QString(Address));
               ArrViewWindow->ui->SettingComboBox->setCurrentIndex(IP_PORT_CHANNAL_parametr);
               ArrViewWindow->ui->RazrComboBox->setCurrentIndex(Param8_16_32);
               ArrViewWindow->ui->VIdComboBox->setCurrentIndex(Param_hex_dec_bin);
               ArrViewWindow->setGeometry(x,y,width,height);
               ArrViewWindow->setWindowTitle("ArraysViewWindow");
               Workspace->addWindow(ArrViewWindow);
               ArrViewWindow->show();
           }
           FileInfoCounter++;
       }
    }
    else QMessageBox::warning(this,"Warning","ErrOfReadfile");
    //if(File.size()==0) QMessageBox::warning(this,"Warning","Last ssesion not found");
}

void ALotOfWindow::ResumeHaltAllProcessors()
{
    QWidgetList WorkspaceWidgetList = Workspace->windowList();
    if(pcStringAction->HaltResumeControlVar==0)
    {
        for(int i=0; i<WorkspaceWidgetList.size();i++)
        {
             if(strcmp(WorkspaceWidgetList.at(i)->metaObject()->className(),"ProcessWindow")==0)
            {
                 ProcessWindow *ProcWind=static_cast<ProcessWindow*>(WorkspaceWidgetList.at(i));
                 ProcWind->SetResume();
            }
        }
        pcStringAction->HaltResumeControlVar=true;
    }
    else
    {
        for(int i=0; i<WorkspaceWidgetList.size();i++)
        {
             if(strcmp(WorkspaceWidgetList.at(i)->metaObject()->className(),"ProcessWindow")==0)
            {
                 ProcessWindow *ProcWind=static_cast<ProcessWindow*>(WorkspaceWidgetList.at(i));
                 ProcWind->SetHalt();
            }
        }
        pcStringAction->HaltResumeControlVar=false;
    }
}
void ALotOfWindow::AllowAutoSaveAction()
{
    ExitAutoSave=true;
}
void ALotOfWindow::BanAutoSaveAction()
{
    ExitAutoSave=false;
}

void ALotOfWindow::SaveOption()
{
    QFile OptionFile("Option.txt");
    if(OptionFile.open(QIODevice::WriteOnly))
    {
        OptionFile.write(QString::number(ExitAutoSave)+"\n");
    }
    OptionFile.close();
}
void ALotOfWindow::RestoreOption()
{
    QFile OptionFile("Option.txt");
    if(OptionFile.open(QIODevice::ReadOnly))
    {
        QString OptionString = OptionFile.readLine();
        int OptionStringCounter=0;
        int AutoSaveInt=0;
        AutoSaveInt = QString(OptionString[OptionStringCounter]).toInt();
        if(AutoSaveInt==1)
        {
            CloseAutSaveAction->YesButton->setChecked(true);
            AllowAutoSaveAction();
        }
        else if(AutoSaveInt==0)
        {
            CloseAutSaveAction->NoButton->setChecked(true);
            BanAutoSaveAction();
        }
    }
    OptionFile.close();
}

void ALotOfWindow::SetAllProcessPC()
{
    QString CurrentPC = pcStringAction->pLabel->text();
    QWidgetList WorkspaceWidgetList = Workspace->windowList();
    for(int i=0; i<WorkspaceWidgetList.size();i++)
    {
         if(strcmp(WorkspaceWidgetList.at(i)->metaObject()->className(),"ProcessWindow")==0)
        {
             ProcessWindow *ProcWind=static_cast<ProcessWindow*>(WorkspaceWidgetList.at(i));
             ProcWind->MovePC(CurrentPC);
        }
    }
}

void ALotOfWindow::CloseAllWindow()
{
    QWidgetList WorkspaceWidgetList = Workspace->windowList();
    for(int i=0; i<WorkspaceWidgetList.size();i++)
    {
        WorkspaceWidgetList.at(i)->close();
    }
}
