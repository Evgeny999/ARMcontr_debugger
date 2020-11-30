#include "celllist1.h"
#include "ui_celllist1.h"
#include "ui_cellvaluechangewindow.h"
#include "mainwindow.h"
#include "rowsettings.h"
#include "ui_rowsettings.h"
#include "ui_afewrowsettings.h"
#include <ctime>


//Set Timer Interval msec


CellList1::CellList1(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CellList1)
{
    Timer=new QTimer(this);
    TableTimerInterval=1000;
    Timer->setInterval(TableTimerInterval);
    connect(Timer,SIGNAL(timeout()),this,SLOT(ReadTableParam()));
    SocketForWrite = new QTcpSocket(this);
    CellSocket = new QTcpSocket(this);
    CulumnBoxCounter=1;
    //CellList->setShortcut(tr("Ctrl+C"));
    ui->setupUi(this);
    ui->statusbar->hide();
    QMenu *menu = menuBar()->addMenu("Add/Remove");
    QAction *Row = new QAction(tr("AddRow"),this);
    Row->setShortcut(tr("Ctrl+R"));
    QAction *RowRemove = new QAction(tr("RemoveRow"),this);
    RowRemove->setShortcut(tr("Alt+R"));
    QAction *Column = new QAction(tr("AddColumn"),this);
    Column->setShortcut(tr("Ctrl+E"));
    QAction *ColumnRemove = new QAction(tr("RemoveColumn"),this);
    ColumnRemove->setShortcut(tr("Alt+E"));
    menu->addAction(Row);
    menu->addAction(Column);
    menu->addAction(RowRemove);
    menu->addAction(ColumnRemove);
    connect(Row,SIGNAL(triggered()),this,SLOT(AddRow()));
    connect(Column,SIGNAL(triggered()),this,SLOT(AddColumn()));
    connect(RowRemove,SIGNAL(triggered()),this,SLOT(RemoveRow()));
    connect(ColumnRemove,SIGNAL(triggered()),this,SLOT(RemoveColumn()));
    //ui->Table->setEditTriggers(QAbstractItemView::AnyKeyPressed);//QAbstractItemView::NoEditTriggers  <=> 0

    QAction *SaveMenu = menuBar()->addAction(tr("SaveSelectedCellToBuffer"));
    connect(SaveMenu,SIGNAL(triggered()),this,SLOT(SaveSelectedCellToBuffer()));


    ui->Table->horizontalHeader()->hide();
    ui->Table->verticalHeader()->hide();
    ui->Table->setRowCount(2);
    ui->Table->setColumnCount(2);

    QLineEdit *TableLine = new QLineEdit(this);
    TableLine->setText("0x00000001");
    TableLine->setBackgroundColor(Qt::green);
    ui->Table->setCellWidget(1,0,TableLine);
    connect(TableLine,SIGNAL(textEdited(QString)),this,SLOT(GetAddressParamList()));
    //TableItem = new QTableWidgetItem("0x00000001");
    //TableItem->setBackgroundColor(Qt::green);
    //ui->Table->setItem(1,0,TableItem);

    TableItem = new QTableWidgetItem("value");
    TableItem->setBackgroundColor(Qt::gray);
    ui->Table->setItem(1,1,TableItem);
    Box=new QComboBox(this);
    GetParamForColumn(Box);
    ui->Table->setCellWidget(0,CulumnBoxCounter,Box);
    CulumnBoxCounter++;
    ui->Table->setItem(0,0,new QTableWidgetItem("|-"));
    ui->Table->item(0,0)->setFlags(!Qt::ItemIsEditable);

    //RowSettingWindow
    SettingWindow = new RowSettings(this);
    SettingWindow->ui->lineEdit->setEnabled(false);
    connect(SettingWindow->ui->lineEdit,SIGNAL(textEdited(QString)),this,SLOT(GetAddressParamList()));
    connect(SettingWindow->ui->Ok,SIGNAL(clicked()),this,SLOT(ChangeRowParam()));
    vbox = new QBoxLayout(QBoxLayout::LeftToRight);
    delete SettingWindow->ui->groupBox->layout();
    SettingWindow->ui->groupBox->setLayout(vbox);
    SettingWindow->ui->comboBox_2->addItem("8",QVariant(8));
    SettingWindow->ui->comboBox_2->addItem("16",QVariant(16));
    SettingWindow->ui->comboBox_2->addItem("32",QVariant(32));

    SettingWindow->ui->comboBox->addItem("hex",QVariant(0));
    SettingWindow->ui->comboBox->addItem("dec",QVariant(1));
    SettingWindow->ui->comboBox->addItem("decSign",QVariant(2));
    SettingWindow->ui->comboBox->addItem("bin",QVariant(3));
    SettingWindow->ui->comboBox->addItem("float",QVariant(4));


    AFewRowSettingsWindow = new AFewRowSettings(this);
    AFewRowSettingsWindow->setWindowTitle("RowsSetiings");
    connect(AFewRowSettingsWindow->ui->OkButton,SIGNAL(clicked()),this,SLOT(ChangeAFewRowsParam()));
    AFewRowSettingsWindow->ui->p81632_comboBox->addItem("8",QVariant(8));
    AFewRowSettingsWindow->ui->p81632_comboBox->addItem("16",QVariant(16));
    AFewRowSettingsWindow->ui->p81632_comboBox->addItem("32",QVariant(32));

    AFewRowSettingsWindow->ui->HexDecBin_comboBox->addItem("hex",QVariant(0));
    AFewRowSettingsWindow->ui->HexDecBin_comboBox->addItem("dec",QVariant(1));
    AFewRowSettingsWindow->ui->HexDecBin_comboBox->addItem("decSign",QVariant(2));
    AFewRowSettingsWindow->ui->HexDecBin_comboBox->addItem("bin",QVariant(3));
    AFewRowSettingsWindow->ui->HexDecBin_comboBox->addItem("float",QVariant(4));


    //32 * размер элемента , поставить размер столбца
    ui->Table->setColumnWidth(0,34*(qobject_cast<QLineEdit*>(ui->Table->cellWidget(1,0)))->fontMetrics().width('0'));

    //Add Context MEnu
    ui->Table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->Table,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(ShowContextMenu(const QPoint&)));

    //SetCheckBox Checked
    for(int i=0;i<RowParamStructCounter;i++)
    {
        for(int j=0;j<20;j++)
            CheckBoxStruct[i].Checked[j]=2;
    }

    //Set start param
    for(int i=0;i<RowParamStructCounter;i++)
    {
        RowParamStruct[i].Count8_16_32=32;
        RowParamStruct[i].HexDecBinCount=0;
        /*  HexDecBin Parametr !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         Hex == 0
         Dec == 1
         DecSign == 2
         Bin == 3
         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         */
    }
    //---------------
    /*Create socket !=null
      (Init socket)
      */
    CellSocket= new QTcpSocket(this);
    //подключаем сигналы от окна настроек строки
    connect(SettingWindow,SIGNAL(send_to_cellList()),this,SLOT(StartTimer()));
    //сигналы для обработки изменений в ячейках
    connect(ui->Table,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(itemChanged(QTableWidgetItem*)));

    this->menuBar();
    menuBar()->addAction("|Частота обновления->");
    menuBar()->addSeparator();
    QAction *Action_1_sec = new QAction("1sec",this);
    connect(Action_1_sec,SIGNAL(triggered()),this,SLOT(SetDelay1Sec()));
    menuBar()->addAction(Action_1_sec);
    QAction *Action_3_sec = new QAction("3sec",this);
    connect(Action_3_sec,SIGNAL(triggered()),this,SLOT(SetDelay3Sec()));
    menuBar()->addAction(Action_3_sec);
    QAction *Action_5_sec = new QAction("5sec |",this);
    connect(Action_5_sec,SIGNAL(triggered()),this,SLOT(SetDelay5Sec()));
    menuBar()->addAction(Action_5_sec);

    ClickedRow=0;
    Timer->start();
}

CellList1::~CellList1()
{
    delete ui;
}
void CellList1::StartTimer()
{
    Timer->start();
}


void CellList1::SetDelay1Sec()
{
    TableTimerInterval = 1000;
    SetTimerInterval();
}
void CellList1::SetDelay3Sec()
{
    TableTimerInterval = 3000;
    SetTimerInterval();
}
void CellList1::SetDelay5Sec()
{
    TableTimerInterval = 5000;
    SetTimerInterval();
}
void CellList1::SetTimerInterval()
{
    Timer->setInterval(TableTimerInterval);
}

//------------------------------------
void CellList1::itemChanged(QTableWidgetItem* ChItem)
{
    /*if Changed address
    changed item = ChItem ;
    Если изменен столбец адрессов , меняяем его на 16ричный формат, так же исключаем ошибки неверного ввода
    Если изменена какя-либо другая ячейка не считая адресной и ячеек содержащих ппараметры подключения
    Смотрим параметры измененного итема , отыскиваем подходящий сокет и изменяем содержимое в соответствующем адрес
    Ищем по структуре изменяемой строки подхъодящие параметры отправки*/
    if(!(ChItem->row()==0))
    {
            Timer->stop();
            if(ChItem->column() == 0) //_______________Change address_______________
            {
             /*bool ok;
             QByteArray AddressChangedInfo;
             int AdressChangedInfo_int;
             AddressChangedInfo=ChItem->text();
             AdressChangedInfo_int=AddressChangedInfo.toInt(&ok,16);
             bool old_blocked = ui->Table->blockSignals(true);
             ChItem->setText(QString().sprintf("0x%08X",AdressChangedInfo_int) );
             ui->Table->blockSignals(old_blocked);*/
            }
            else //________________________________change cell value_______________________________________
            {
                //Block table signals
                bool old_blocked = ui->Table->blockSignals(true);
                bool ok;
                bool WriteSuccess = false;
                int ItemColumn = ChItem->column();
                int ItemRow = ChItem->row();
                signed char Char=0;
                signed short Short=0;
                signed int Int=0;

                float WriteValue_float;
                //Search write add
                QString WriteAddress;
                QLineEdit *AddLine = qobject_cast<QLineEdit*>(ui->Table->cellWidget(ItemRow,0));
                WriteAddress=AddLine->text();
                //-----------------------------------------------------------------------------------
                unsigned int WriteAdd_int;
                if((WriteAddress[0]=='0')&&(WriteAddress[1]=='x'))
                {
                    //SetAddress=(RowParamStruct[RowCounter].Address).toInt(&ok1,16);
                    WriteAdd_int = WriteAddress.toInt(&ok,16);
                }
                else
                {
                   varlistelm_t *RecombAddress = new varlistelm_t;
                   bool update=false;
                   dbal_fillfromdb(WriteAddress,RecombAddress,update);
                   WriteAdd_int=RecombAddress->addr;
               }
                //Search write value
                QString WriteValue = ChItem->text();
                WriteValue.replace(" ","");
                int WriteValue_int=0;
                //Repl value into hex dec bin
                if(RowParamStruct[ItemRow-1].HexDecBinCount==0)
                {
                 WriteValue_int = WriteValue.toInt(&ok,16);
                }
                if(RowParamStruct[ItemRow-1].HexDecBinCount==1)
                {
                  WriteValue_int = WriteValue.toInt(&ok,10);
                }
                if(RowParamStruct[ItemRow-1].HexDecBinCount==2)
                {
                    Char=WriteValue.toInt(&ok,10);
                    Short=WriteValue.toInt(&ok,10);
                    Int=WriteValue.toInt(&ok,10);
                }
                if(RowParamStruct[ItemRow-1].HexDecBinCount==3)
                {
                  WriteValue_int = WriteValue.toInt(&ok,2);
                }

                if(RowParamStruct[ItemRow-1].HexDecBinCount==4)
                {
                  WriteValue_float = WriteValue.toFloat(&ok);
                  int *float_to_int = (int*)(&WriteValue_float);
                  WriteValue_int = *float_to_int ;
                }


                //Search Socket
                int QuantitiIterator=0;
                //Search socket
                while(!Quantiti[QuantitiIterator].Flag==0)
                {
                    if(ColumnParalStruct[ItemColumn-1].ColumnVirtualCounter==Quantiti[QuantitiIterator].VirtualCounter)
                    {
                       SocketForWrite=&(Quantiti[QuantitiIterator].IpPortSocket);
                    }
                    QuantitiIterator++;
                }
                //IF connected, write value
                if(SocketForWrite->state()==QAbstractSocket::ConnectedState)
                {
                    /*  HexDecBin Parametr !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                     Hex == 0
                     Dec == 1
                     DecSign == 2
                     Bin == 3
                     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                     */
                    //Check value simbols
                    bool hex = true;
                    bool dec = true;
                    bool bin = true;
                       //Check Bin
                       for(int InfoIterator=0;InfoIterator<WriteValue.size();InfoIterator++)
                        {
                           if(!((WriteValue[InfoIterator]=='0')||(WriteValue[InfoIterator]=='1')))
                           {
                               bin = false;
                               break;
                           }
                        }
                       //Check Dec
                       for(int InfoIterator=0;InfoIterator<WriteValue.size();InfoIterator++)
                       {
                           if(!((WriteValue[InfoIterator]=='0')||(WriteValue[InfoIterator]=='1')||(WriteValue[InfoIterator]=='2')||(WriteValue[InfoIterator]=='3')||(WriteValue[InfoIterator]=='4')||(WriteValue[InfoIterator]=='5')||(WriteValue[InfoIterator]=='6')||(WriteValue[InfoIterator]=='7')||(WriteValue[InfoIterator]=='8')||(WriteValue[InfoIterator]=='9')))
                           {
                               dec=false;
                               break;
                           }
                       }
                       //Check Hex
                       for(int InfoIterator=0;InfoIterator<WriteValue.size();InfoIterator++)
                       {
                           if(!((WriteValue[InfoIterator]=='0')||(WriteValue[InfoIterator]=='1')||(WriteValue[InfoIterator]=='2')||(WriteValue[InfoIterator]=='3')||(WriteValue[InfoIterator]=='4')||(WriteValue[InfoIterator]=='5')||(WriteValue[InfoIterator]=='6')||(WriteValue[InfoIterator]=='7')||(WriteValue[InfoIterator]=='8')||(WriteValue[InfoIterator]=='9')||(WriteValue[InfoIterator]=='A')||(WriteValue[InfoIterator]=='a')||(WriteValue[InfoIterator]=='B')||(WriteValue[InfoIterator]=='b')||(WriteValue[InfoIterator]=='C')||(WriteValue[InfoIterator]=='c')||(WriteValue[InfoIterator]=='D')||(WriteValue[InfoIterator]=='d')||(WriteValue[InfoIterator]=='E')||(WriteValue[InfoIterator]=='e')||(WriteValue[InfoIterator]=='F')||(WriteValue[InfoIterator]=='f')))
                           {
                               hex=false;
                               break;
                           }
                       }


                       if((RowParamStruct[ItemRow-1].HexDecBinCount==0)&&(hex==true))//IF hex
                       {
                                if(RowParamStruct[ItemRow-1].Count8_16_32==8)
                                {
                                    if(WriteValue.size()<=2)// Chech value size
                                    SocketForWrite->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
                                }
                                if(RowParamStruct[ItemRow-1].Count8_16_32==16)
                                {
                                    if(WriteValue.size()<=4)// Chech value size
                                    SocketForWrite->write(QString().sprintf("capture \"mwh 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
                                }
                                if(RowParamStruct[ItemRow-1].Count8_16_32==32)
                                {
                                    if(WriteValue.size()<=8)// Chech value size
                                    SocketForWrite->write(QString().sprintf("capture \"mww 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
                                }
                        }
                        else if((RowParamStruct[ItemRow-1].HexDecBinCount==1)&&(dec==true))//if dec
                        {
                                if(RowParamStruct[ItemRow-1].Count8_16_32==8)
                                {
                                    if(WriteValue.size()<=3)// Chech value size
                                    SocketForWrite->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
                                }
                                if(RowParamStruct[ItemRow-1].Count8_16_32==16)
                                {
                                    if(WriteValue.size()<=5)// Chech value size
                                    SocketForWrite->write(QString().sprintf("capture \"mwh 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
                                }
                                if(RowParamStruct[ItemRow-1].Count8_16_32==32)
                                {
                                    if(WriteValue.size()<=10)// Chech value size
                                    SocketForWrite->write(QString().sprintf("capture \"mww 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
                                }
                        }
                        else if(RowParamStruct[ItemRow-1].HexDecBinCount== 2)//if dec sign
                        {
                            if(RowParamStruct[ItemRow-1].Count8_16_32==8)
                            {
                                if(WriteValue.size()<=8)// Chech value size
                                SocketForWrite->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",WriteAdd_int,Char));
                            }
                            if(RowParamStruct[ItemRow-1].Count8_16_32==16)
                            {
                                if(WriteValue.size()<=16)// Chech value size
                                SocketForWrite->write(QString().sprintf("capture \"mwh 0x%X 0x%X\"\x1A",WriteAdd_int,Short));
                            }
                            if(RowParamStruct[ItemRow-1].Count8_16_32==32)
                            {
                                if(WriteValue.size()<=32)// Chech value size
                                SocketForWrite->write(QString().sprintf("capture \"mww 0x%X 0x%X\"\x1A",WriteAdd_int,Int));
                            }
                        }
                        else if((RowParamStruct[ItemRow-1].HexDecBinCount==3)&&(bin==true))//if bin
                        {
                            if(RowParamStruct[ItemRow-1].Count8_16_32==8)
                            {
                                if(WriteValue.size()<=8)// Chech value size
                                SocketForWrite->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
                            }
                            if(RowParamStruct[ItemRow-1].Count8_16_32==16)
                            {
                                if(WriteValue.size()<=16)// Chech value size
                                SocketForWrite->write(QString().sprintf("capture \"mwh 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
                            }
                            if(RowParamStruct[ItemRow-1].Count8_16_32==32)
                            {
                                if(WriteValue.size()<=32)// Chech value size
                                SocketForWrite->write(QString().sprintf("capture \"mww 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
                            }
                        }
                        else if((RowParamStruct[ItemRow-1].HexDecBinCount==4))//if dec
                        {
                                if(RowParamStruct[ItemRow-1].Count8_16_32==8)
                                {
                                    //if(WriteValue.size()<=3)// Chech value size
                                    SocketForWrite->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
                                }
                                if(RowParamStruct[ItemRow-1].Count8_16_32==16)
                                {
                                    //if(WriteValue.size()<=5)// Chech value size
                                    SocketForWrite->write(QString().sprintf("capture \"mwh 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
                                }
                                if(RowParamStruct[ItemRow-1].Count8_16_32==32)
                                {
                                    //if(WriteValue.size()<=10)// Chech value size
                                    SocketForWrite->write(QString().sprintf("capture \"mww 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
                                }
                        }

                    WriteSuccess = true;
                }
                QByteArray BBB;
                QByteArray ChannelCheck;
                bool CheackSearch=false;
                //If write succsesful, cntrol read
                if(WriteSuccess==true)
                {
                        while((SocketForWrite->bytesAvailable())||(SocketForWrite->waitForReadyRead(1000)))
                        {
                            SocketForWrite->waitForReadyRead(100);
                            BBB=SocketForWrite->peek(100);
                            for(int i=0;i<BBB.size();i++)
                            {
                                if(BBB[i]=='\x1A')
                                {
                                    ChannelCheck=SocketForWrite->read(i+1);
                                    CheackSearch=true;
                                    break;
                                }
                            }
                            if(CheackSearch==true) break;
                            BBB.clear();
                        }
                 }
                //unblock table signals
                ui->Table->blockSignals(old_blocked);
            }
            Timer->start();
    }
}

void CellList1::AddRow()
{
    //блокируем сигнал, реагирующий на изменение в строках на время добавления
    bool old_blocked = ui->Table->blockSignals(true);

    QLineEdit* TableLine = new QLineEdit(this);
    TableLine->setText(QString().sprintf("0x%08X ",ui->Table->rowCount()));
    connect(TableLine,SIGNAL(textEdited(QString)),this,SLOT(GetAddressParamList()));
    TableLine->setBackgroundColor(Qt::green);
    if(ui->Table->selectionModel()->currentIndex().row()>=1)
    {
        ui->Table->insertRow(ui->Table->selectionModel()->currentIndex().row()+1);
        for(int j=ui->Table->rowCount();j>=ui->Table->selectionModel()->currentIndex().row();j--)
        {
            if(j!=RowParamStructCounter) RowParamStruct[j+1]=RowParamStruct[j];
        }
        ui->Table->setCellWidget(ui->Table->selectionModel()->currentIndex().row()+1,0,TableLine);

        for(int colcount=1;colcount<=ui->Table->columnCount();colcount++)
        {
            for(int rowcount=1;rowcount<=ui->Table->rowCount();rowcount++)
            {
                if(!(ui->Table->item(rowcount,colcount)))
                {
                    TableItem = new QTableWidgetItem("value");
                    TableItem->setBackgroundColor(Qt::gray);
                    ui->Table->setItem(rowcount,colcount,TableItem);
                }
             }
        }
    }
    //снимамем блок с сигнала
    ui->Table->blockSignals(old_blocked);
}
void CellList1::AddRowInBack()
{
    bool old_blocked = ui->Table->blockSignals(true);
    QLineEdit* TableLine = new QLineEdit(this);
    TableLine->setText(QString().sprintf("0x%08X ",ui->Table->rowCount()));
    connect(TableLine,SIGNAL(textEdited(QString)),this,SLOT(GetAddressParamList()));
    TableLine->setBackgroundColor(Qt::green);
    ui->Table->insertRow(ui->Table->rowCount());
    ui->Table->setCellWidget(ui->Table->rowCount()-1,0,TableLine);

    for(int colcount=1;colcount<=ui->Table->columnCount();colcount++)
    {
        if(!(ui->Table->item(ui->Table->rowCount()-1,colcount)))
        {
        TableItem = new QTableWidgetItem("value");
        TableItem->setBackgroundColor(Qt::gray);
        ui->Table->setItem(ui->Table->rowCount()-1,colcount,TableItem);
        }
    }
    //снимамем блок с сигнала
    ui->Table->blockSignals(old_blocked);
}

void CellList1::AddColumn()
{
    bool old_blocked = ui->Table->blockSignals(true); //block signals
    ui->Table->insertColumn(ui->Table->columnCount());
    Box=new QComboBox(this);
    GetParamForColumn(Box);
    ui->Table->setCellWidget(0,ui->Table->columnCount()-1,Box);
    for(int rowcount=1;rowcount<=ui->Table->rowCount();rowcount++)
    {
        if(!(ui->Table->item(rowcount,ui->Table->columnCount()-1)))
        {
        TableItem = new QTableWidgetItem("value");
        TableItem->setBackgroundColor(Qt::gray);
        ui->Table->setItem(rowcount,ui->Table->columnCount()-1,TableItem);
        }
    }
    CulumnBoxCounter++;
    ui->Table->blockSignals(old_blocked);//unblock signals
}
void CellList1::RemoveColumn()
{
    QComboBox *DellWidget;
    if(ui->Table->selectionModel()->hasSelection())
    if(ui->Table->columnCount()>2)
    {
        DellWidget =qobject_cast<QComboBox*>(ui->Table->cellWidget(0,ui->Table->selectionModel()->currentIndex().column()));
        DellWidget->blockSignals(true);
        DellWidget->hide();
        ui->Table->removeColumn(ui->Table->selectionModel()->currentIndex().column());
    }
}
void CellList1::RemoveRow()
{
    QList<QTableWidgetItem*> ItemList = ui->Table->selectedItems();
    if(ui->Table->rowCount()>2)
    {
        if((ui->Table->rowCount()-ItemList.size())>2)
        {
            for(int i=0; i<ItemList.size();i++)
            {
                RecombRow_Param_Struct(ItemList.at(i)->row());
                ui->Table->removeRow(ItemList.at(i)->row());
            }
        }
    }
}
void CellList1::RecombRow_Param_Struct(int Row)
{
    for(int i=Row;i<ui->Table->rowCount();i++)
    {
        if(i<=RowParamStructCounter) RowParamStruct[i-1]=RowParamStruct[i];
    }
}


void CellList1::GetParamForColumn(QComboBox *BoxItem)
{
    //setWidth both
    qobject_cast<QListView*>(BoxItem->view())->setMinimumWidth(350);
    //set Aligment
    BoxItem->setEditable(true);
    BoxItem->lineEdit()->setReadOnly(true);
    BoxItem->lineEdit()->setAlignment(Qt::AlignCenter);
    //SetAddr
    QString Name;
    int i=0;
    int NameCounter=0;
    int NumOfCh=0;
    QString CostomNameMass[MaxNumberOfCostomName];
    int CostomNameIterator=0;
    int CostomNameMassIterator=0;
    int j=0;
    BoxItem->addItem("ConnectionSetting");
    BoxItem->addItem("___________________________________");    
    while(Quantiti[i].Flag!=0)
    {
        NumOfCh=Quantiti[i].NumberOfChannal;
        //CostomName
        for(int massClearCounter=0;massClearCounter<MaxNumberOfCostomName;massClearCounter++)
        {
            CostomNameMass[massClearCounter].clear();
        }
        CostomNameIterator=0;
        CostomNameMassIterator=0;
        j=0;
        while(Quantiti[i].CustomName[CostomNameIterator]!='\0')
        {
            while((Quantiti[i].CustomName[CostomNameIterator]!=',')&&(Quantiti[i].CustomName[CostomNameIterator]!='\0'))
            {
                CostomNameMass[CostomNameMassIterator][j]=Quantiti[i].CustomName[CostomNameIterator];
                j++;
                CostomNameIterator++;
            }
            if(Quantiti[i].CustomName[CostomNameIterator]!='\0')
            {
                j=0;
                CostomNameMassIterator++;
                CostomNameIterator++;
            }
        }

        for(int h=0;h<NumOfCh;h++)
        {
        Name.clear();
        Name=QString("IP:%0 PORT:%1 Channel:%2 Name:%4 ").arg(Quantiti[i].IpAddr).arg(QString::number(Quantiti[i].Port)).arg(QString::number(h)).arg(CostomNameMass[h]);
        NameMass[NameCounter]=Name;
        BoxItem->addItem(Name,QVariant(Quantiti[i].VirtualCounter));
        NameCounter++;
        }
        i++;
        BoxItem->addItem("----------------------------------------");
    }
    connect(Box,SIGNAL(currentIndexChanged(int)),this,SLOT(ColumnChangeParam(int)));
}

void CellList1::ColumnChangeParam(int index)
{
    /*IP.clear();
    PORT=0;
    int i=0;
    int C1=0;
    QString Par;
    QString PortChanPr;
    int value=Box->itemData(index).toInt();
    //qDebug()<<NameMass[value];
    Par=NameMass[value];
    while(Par[i]!='\0')
    {
        if((Par[i]=='I')&&(Par[i+1]=='P')&&(Par[i+2]==':'))
        {
            i=i+3;
            while(Par[i]!=' ')
            {
                IP[C1]=Par[i];
                C1++;
                i++;
            }
            C1=0;
        }
        if((Par[i]=='P')&&(Par[i+1]=='O')&&(Par[i+2]=='R')&&(Par[i+3]=='T')&&(Par[i+4]==':'))
        {
            i=i+5;
            while(Par[i]!=' ')
            {
                PortChanPr[C1]=Par[i];
                C1++;
                i++;
            }
            PORT=PortChanPr.toInt();
            C1=0;
            PortChanPr.clear();
        }

        if((Par[i]=='C')&&(Par[i+1]=='h')&&(Par[i+2]=='a')&&(Par[i+3]=='n')&&(Par[i+4]=='n')&&(Par[i+5]=='e')&&(Par[i+6]=='l')&&(Par[i+7]==':'))
        {
            i=i+8;
            while(Par[i]!=' ')
            {
               PortChanPr[C1]=Par[i];
                C1++;
                i++;
            }
            CHANNAL=PortChanPr.toInt();
            C1=0;
            PortChanPr.clear();
        }
        i++;
    }*/
}

//Context Menu
void CellList1::ShowContextMenu(const QPoint &pos)
{
    QPoint globalPos=ui->centralwidget->mapToGlobal(pos);
    ContextMenu = new QMenu(this);
    //ContextMenu->addAction("RowParametr",this,SLOT(ContextMenuChangeRowParam()));
    ContextMenu->exec(globalPos);
    ContextMenuChangeRowParam();
}
//If contextmenu ChangeRowParam clicked
void CellList1::ContextMenuChangeRowParam()
{
    QList<QTableWidgetItem*> ItemList = ui->Table->selectedItems();
    if(ItemList.size()==1)
    {
        int row = ui->Table->selectionModel()->currentIndex().row();
        int column = ui->Table->selectionModel()->currentIndex().column();
        if(((row==0)&&(column==0))||(column==0)||(column<0)||(row<0))
        {

        }
        else
        {
            Timer->stop();
            ClickedRow=row;
            int SetCurrent8_16_32=0;
            //delete check box
            QObjectList children = SettingWindow->ui->groupBox->children();
            QCheckBox *check;
            foreach(QObject *child,children)
            {
                check=qobject_cast<QCheckBox*>(child);
                if(check) {
                    delete check;
                }
            }
            //add new checkbox
            for(int i=1 ; i<ui->Table->columnCount();i++)
            {
                QCheckBox *Check = new QCheckBox(this);
                if(CheckBoxStruct[ClickedRow].Checked[i-1]==2) Check->setCheckState(Qt::Checked);
                else Check->setCheckState(Qt::Unchecked);
                vbox->addWidget(Check);
                //vbox->addStretch(1);
            }
            QLineEdit* TableLine = new QLineEdit(this);
            TableLine=qobject_cast<QLineEdit*>(ui->Table->cellWidget(row,0));
            QString PropertAddress;
            PropertAddress=TableLine->text();
            int SetAddress;
            //SettingWindow->ui->lineEdit->setText(TableLine->text());
            if((PropertAddress[0]=='0')&&(PropertAddress[1]=='x'))
            {
                SettingWindow->ui->lineEdit->setText(PropertAddress);
                SettingWindow->ui->label->setText("Address:");
            }
            else
            {
               varlistelm_t *RecombAddress = new varlistelm_t;
               bool update=false;
               dbal_fillfromdb(PropertAddress,RecombAddress,update);
               SetAddress=RecombAddress->addr;
               SettingWindow->ui->lineEdit->setText(PropertAddress);
               SettingWindow->ui->label->setText(QString().sprintf("Address:0x%08X",SetAddress));
            }

            SettingWindow->setWindowTitle("Row: "+QString::number(row)+QString(" Settings"));
            //Validator
            //SettingWindow->ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Fa-f0-9x]{8}"),this));
            //ComboBoxStartParam
            if(RowParamStruct[ClickedRow-1].Count8_16_32==8) SetCurrent8_16_32=0;
            if(RowParamStruct[ClickedRow-1].Count8_16_32==16) SetCurrent8_16_32=1;
            if(RowParamStruct[ClickedRow-1].Count8_16_32==32) SetCurrent8_16_32=2;
            SettingWindow->ui->comboBox_2->setCurrentIndex(SetCurrent8_16_32);
            SettingWindow->ui->comboBox->setCurrentIndex(RowParamStruct[ClickedRow-1].HexDecBinCount);

            SettingWindow->show();
        }
    }
    else if(ItemList.size()>1)
    {
        AFewRowSettingsWindow->RowsInfoList=ItemList;
        AFewRowSettingsWindow->ui->p81632_comboBox->setCurrentIndex(2);
        AFewRowSettingsWindow->ui->HexDecBin_comboBox->setCurrentIndex(0);
        AFewRowSettingsWindow->show();
    }
}
//If SettingWindow button clicked
void CellList1::ChangeRowParam()
{
  // блокируем сигнал, реагирующий на изменение в итеме
   bool old_blocked = ui->Table->blockSignals(true);
   bool ok;
   QString LAddr;
   int LocalAddress;
   int P8_16_32;
   int PHex_Dec_bin;
   LAddr=SettingWindow->ui->lineEdit->text();
   //QString().sprintf("0x%08X: ",LocalAddress)
   LocalAddress=(LAddr).toInt(&ok,16);
   QLineEdit * TableLine = new QLineEdit(this);
   TableLine = qobject_cast<QLineEdit*>(ui->Table->cellWidget(ClickedRow,0));
   if((LAddr[0]=='0')&&(LAddr[1]=='x'))
   {
        TableLine->setText(QString().sprintf("0x%08X ",LocalAddress));
   }
   else
   {
       TableLine->setText(LAddr);
   }

   P8_16_32=SettingWindow->ui->comboBox_2->itemData(SettingWindow->ui->comboBox_2->currentIndex()).toInt();
   RowParamStruct[ClickedRow-1].Count8_16_32=P8_16_32;
   PHex_Dec_bin=SettingWindow->ui->comboBox->itemData(SettingWindow->ui->comboBox->currentIndex()).toInt();
   RowParamStruct[ClickedRow-1].HexDecBinCount=PHex_Dec_bin;
   //Search and check checkbox for row
   QObjectList children = SettingWindow->ui->groupBox->children();
   QCheckBox *check;
   int CheckCounter=0;
   foreach(QObject *child,children)
   {
       check=qobject_cast<QCheckBox*>(child);
       if(check) {
           CheckBoxStruct[ClickedRow].Checked[CheckCounter]=check->checkState();
           CheckCounter++;
       }
   }
   //qDebug()<<"check1";
   for(int CellState=0;CellState<ui->Table->columnCount()-1;CellState++)
   {
       if(CheckBoxStruct[ClickedRow].Checked[CellState]==0) ui->Table->item(ClickedRow,CellState+1)->setText("-");
       if(CheckBoxStruct[ClickedRow].Checked[CellState]==2) ui->Table->item(ClickedRow,CellState+1)->setText("");
   }
   //выключаем блок сигнала
   ui->Table->blockSignals(old_blocked);
   SettingWindow->hide();
   Timer->start();
}

void CellList1::ChangeAFewRowsParam()
{
    int CurrentRow=2;
    int Current81632Setting= AFewRowSettingsWindow->ui->p81632_comboBox->itemData(AFewRowSettingsWindow->ui->p81632_comboBox->currentIndex()).toInt();
    int CurrentHexDecBinSetting = AFewRowSettingsWindow->ui->HexDecBin_comboBox->itemData(AFewRowSettingsWindow->ui->HexDecBin_comboBox->currentIndex()).toInt();
    for(int i=0;i<AFewRowSettingsWindow->RowsInfoList.size();i++)
    {
        CurrentRow=AFewRowSettingsWindow->RowsInfoList.at(i)->row();
        RowParamStruct[CurrentRow-1].Count8_16_32=Current81632Setting;
        RowParamStruct[CurrentRow-1].HexDecBinCount=CurrentHexDecBinSetting;
    }
    AFewRowSettingsWindow->hide();
}

void CellList1::ReadTableParam()
{
    //qDebug()<<ui->Table->cellWidget(0,1)->children();
    Timer->stop();
    // QString ColumnParamString;
    //Формируем структуры для работы сл столбцами и строками
    //заполнение структур строк
    int rowcolumnnumber=0;
    for(rowcolumnnumber=1;rowcolumnnumber<ui->Table->rowCount();rowcolumnnumber++)
    {
        QLineEdit *TableLine=qobject_cast<QLineEdit*>(ui->Table->cellWidget(rowcolumnnumber,0));
        QString Address1 = TableLine->text();
        RowParamStruct[rowcolumnnumber-1].Address=Address1;

       //QLineEdit *TableLine=qobject_cast<QLineEdit*>(ui->Table->cellWidget(rowcolumnnumber,0));
       //RowParamStruct[rowcolumnnumber-1].Address=TableLine->text();
       RowParamStruct[rowcolumnnumber-1].ExitFlag=0;
     }
    RowParamStruct[rowcolumnnumber-1].ExitFlag=1;

    rowcolumnnumber=0;
    QComboBox *BoxElemInTable=new QComboBox(this);
    QString Par;
    QString PortChanPr;
    int C1=0;
    int i=0;
    //Заполнение структур столбцов
    for(rowcolumnnumber=1;rowcolumnnumber<ui->Table->columnCount();rowcolumnnumber++)
    {
        i=0;
        int Control=0;//If IP PORT and Channal was fouded -> Control=3, and ColumnParam->ExitFlag=0
        Par.clear();
        BoxElemInTable=qobject_cast<QComboBox*>(ui->Table->cellWidget(0,rowcolumnnumber));
        //Узнаем индекс столбца по заданному QVariant
        int Index = BoxElemInTable->itemData(BoxElemInTable->currentIndex()).toInt();
        Par=BoxElemInTable->currentText();
        //read
        while(Par[i]!='\0')
        {
            if((Par[i]=='I')&&(Par[i+1]=='P')&&(Par[i+2]==':'))
            {
                i=i+3;
                while(Par[i]!=' ')
                {
                    ColumnParalStruct[rowcolumnnumber-1].ColumnIpAddress[C1]=Par[i];
                    C1++;
                    i++;
                }
                C1=0;
                Control++;
            }
            //else{ColumnParalStruct[rowcolumnnumber-1].ExitFlag=1;}

            if((Par[i]=='P')&&(Par[i+1]=='O')&&(Par[i+2]=='R')&&(Par[i+3]=='T')&&(Par[i+4]==':'))
            {
                i=i+5;
                while(Par[i]!=' ')
                {
                    PortChanPr[C1]=Par[i];
                    C1++;
                    i++;
                }
                Control++;
                ColumnParalStruct[rowcolumnnumber-1].ColumnPORT=PortChanPr.toInt();
                C1=0;
                PortChanPr.clear();
                ColumnParalStruct[rowcolumnnumber-1].ColumnVirtualCounter=Index;
            }
            if((Par[i]=='C')&&(Par[i+1]=='h')&&(Par[i+2]=='a')&&(Par[i+3]=='n')&&(Par[i+4]=='n')&&(Par[i+5]=='e')&&(Par[i+6]=='l')&&(Par[i+7]==':'))
            {
                i=i+8;
                while(Par[i]!=' ')
                {
                   PortChanPr[C1]=Par[i];
                    C1++;
                    i++;
                }
                Control++;
                ColumnParalStruct[rowcolumnnumber-1].ColumnChannel=PortChanPr.toInt();
                C1=0;
                ColumnParalStruct[rowcolumnnumber-1].ExitFlag=0;
                PortChanPr.clear();
            }
            i++;
        }
        Control=0;
    }
    ColumnParalStruct[rowcolumnnumber-1].ExitFlag=1;
    /*qDebug()<<"1---------------------------------------------------1";
    int Proverka=0;
    while(!(ColumnParalStruct[Proverka].ExitFlag==1))
    {
        qDebug()<<ColumnParalStruct[Proverka].ColumnIpAddress;
        qDebug()<<ColumnParalStruct[Proverka].ColumnPORT;
        qDebug()<<ColumnParalStruct[Proverka].ColumnChannel;
        qDebug()<<ColumnParalStruct[Proverka].ColumnVirtualCounter;
        Proverka++;
    }
    qDebug()<<"2---------------------------------------------------2";*/
    ConnectAndFormCommand();
}

void CellList1::ConnectAndFormCommand()
{
    /*
      Идем по структуре столбцов, смотрим виртуальные счетчики, сравниваем счетчики со структурами, в которые входят структуры с сокетами
      Смотрим подключен ли сокет к айпи адрессу. Если нет подключить. Да выполнить запись и чтение. В состоянии подключения пропустить
      Проверить соответствие канала сокету
    */
    for(int ColumnCounter=0; ColumnCounter<ui->Table->columnCount()-1;ColumnCounter++)
    {
        //Если существует 1 элемент в комбобоксе и он некорректно задан(т.е айпи ,порт ,канал не заданны) выйти из цыкла
        if(ColumnParalStruct[ColumnCounter].ExitFlag==1) break;
        //-----------------
        int QuantitiIterator=0;
        //Ищем подходящий сокет, определяем параметр для проверки переключения канала
        while(!Quantiti[QuantitiIterator].Flag==0)
        {
            //if(ColumnParalStruct[ColumnCounter].ColumnVirtualCounter==Quantiti[QuantitiIterator].VirtualCounter)
            if((ColumnParalStruct[ColumnCounter].ColumnIpAddress==Quantiti[QuantitiIterator].IpAddr)&&(ColumnParalStruct[ColumnCounter].ColumnPORT==Quantiti[QuantitiIterator].Port))
            {
                CellSocket=&(Quantiti[QuantitiIterator].IpPortSocket);
                NumberOfChannelControl=Quantiti[QuantitiIterator].NumberOfChannal;
            }
            QuantitiIterator++;
        }
            if(CellSocket)
        switch(CellSocket->state())
        {
        case(QAbstractSocket::ConnectedState):
             HaltResumeStateCheck(ColumnCounter);
             FormAndSendCommand(ColumnCounter);
             //ReadInfoFromSocket();
             DataFilter();
             InsertInfoIntoCell(ColumnCounter);
            break;
        case(QAbstractSocket::UnconnectedState):
            {
            CellSocket->connectToHost(QString("%0").arg(ColumnParalStruct[ColumnCounter].ColumnIpAddress),ColumnParalStruct[ColumnCounter].ColumnPORT);
            QComboBox *CurrentBox = new QComboBox();
            CurrentBox = qobject_cast<QComboBox*>(ui->Table->cellWidget(0,ColumnCounter+1));
            CurrentBox->setBackgroundColor(Qt::white);
            }
            break;
        case(QAbstractSocket::ConnectingState):{
            bool old_blocked = ui->Table->blockSignals(true);
            for(int i=1;i<ui->Table->rowCount();i++) {ui->Table->item(i,ColumnCounter+1)->setText("connecting"); ui->Table->item(i,ColumnCounter+1)->setBackgroundColor(Qt::yellow);}
            ui->Table->blockSignals(old_blocked);
            break;
        }
        default:
            break;
        }
    }
    Timer->start();
}

void CellList1::FormAndSendCommand(int ColumnCounter)
{
    //qDebug()<<"FormAndSendCommand";
    //Переключение канала работы и проверка переключения
    QByteArray BBB;
    QByteArray ChannelCheck;
    bool CheackSearch=false;
    bool ok1;
    unsigned int SetAddress;
    /*проверка на то, что каналов больше чем 1
    на случай если подсоеденено 1 устройство*/
    if(NumberOfChannelControl>1)
    {
    CellSocket->write(QString().sprintf("capture \"pcilink_setchan %d\"\x1A",ColumnParalStruct[ColumnCounter].ColumnChannel));
    ChannelCheck.clear();
    //qDebug()<<"FormAndSendCommand-readChannal";
    while(ChannelCheck.size()<=13)
    {
        //Ждем 5 сек и отключаемся если socket не отвечает
        if(CellSocket->waitForReadyRead(5000)){}
        else
        {
            CellSocket->disconnectFromHost();
            break;
        }
        //Reading
       // CellSocket->waitForReadyRead(100);
        BBB=CellSocket->peek(13);
        for(int i=0;i<BBB.size();i=i+1)
        {
            if(BBB[i]=='\x1A')
            {
                ChannelCheck=CellSocket->read(i=i+1);
                CheackSearch=true;
                break;
            }
        }
        if(CheackSearch==true) break;
        BBB.clear();
    }
    }else CheackSearch=true;

    if((CheackSearch==true)&&(CellSocket->state()==QAbstractSocket::ConnectedState))
    {
         RowCount=ui->Table->rowCount()-1;
         //qDebug()<<clock()<<"---------WriteStart";
         SocketData.clear();
        for(int RowCounter=0;RowCounter<RowCount;RowCounter++)
        {
            if((RowParamStruct[RowCounter].Address[0]=='0')&&(RowParamStruct[RowCounter].Address[1]=='x'))
            {
                SetAddress=(RowParamStruct[RowCounter].Address).toInt(&ok1,16);
            }
            else
            {
               varlistelm_t *RecombAddress = new varlistelm_t;
               bool update=false;
               dbal_fillfromdb(RowParamStruct[RowCounter].Address,RecombAddress,update);
               SetAddress=RecombAddress->addr;
            }
            if(RowParamStruct[RowCounter].Count8_16_32==8)
            {
                CellSocket->write(QString().sprintf("capture \"mdb 0x%X\"\x1A",SetAddress));
                ReadInfoFromSocket();
            }
            if(RowParamStruct[RowCounter].Count8_16_32==16)
            {
                CellSocket->write(QString().sprintf("capture \"mdh 0x%X\"\x1A",SetAddress));
                ReadInfoFromSocket();
            }
            if(RowParamStruct[RowCounter].Count8_16_32==32)
            {
                CellSocket->write(QString().sprintf("capture \"mdw 0x%X\"\x1A",SetAddress));
                ReadInfoFromSocket();
            }

            if((RowCounter==0)&&(SocketData.contains("Invalid",Qt::CaseInsensitive))) return;
        }
    }
}

void CellList1::ReadInfoFromSocket()
{
    //SocketData.clear();
    bool exitCounter=false;
    int iter=0;
    QByteArray SocReadInfo;
    while(SocketData.size()<=RowCount*200)
    {
        //CellSocket->waitForReadyRead(100);
        //Ждем 5 сек и отключаемся если socket не отвечает
        if(CellSocket->waitForReadyRead(5000)){}
        else
        {
            SocketData.clear();
            CellSocket->disconnectFromHost();
            break;
        }
        //Reading
        SocReadInfo=CellSocket->peek(CellSocket->bytesAvailable());
        for(iter=0;iter<SocReadInfo.size();iter++)
        {
            if(SocReadInfo[iter]=='\x1A')
            {
                exitCounter=true;
                SocketData.append(CellSocket->read(iter=iter+1));
                break;
            }
        }
        SocReadInfo.clear();
        if(exitCounter==true) break;
    }
}

void CellList1::DataFilter()
{
    int LocalCounter=0;
    int NumOfLocalData=0;
    int NumOfStringData=0;
    int RowCounter=0;
    int i;
    int DataPr2=0;
    int ZeroSize;
    char DataPr3[40];
    QString DataPr;
    QString AddZero;
    QString DataBit;
    bool ok;
    float *FloatData;

    //Clear RowData
    for(int i=0;i<RowParamStructCounter;i++)
    {
    RowData[i].clear();
    }

    while(SocketData[NumOfLocalData]!='\0')
    {
        if(SocketData[NumOfLocalData]=='\n')
        {
            LocalCounter=0;
            RowCounter++;
            NumOfStringData=0;
        }

        if(LocalCounter==1)
        {
            RowData[RowCounter][NumOfStringData]=SocketData[NumOfLocalData];
            NumOfStringData++;
        }

        if(SocketData[NumOfLocalData]==':')
        {
            LocalCounter=1;
            NumOfLocalData++;
        }
        NumOfLocalData++;
    }
    /*Write in last string "fin"*/
    RowData[RowCounter][0]='f';RowData[RowCounter][1]='i';RowData[RowCounter][2]='n';

    RowCounter = 0;
    /*while !lastString replicate row info in hex_dec_bin*/
    while(!((RowData[RowCounter][0]=='f')&&(RowData[RowCounter][1]=='i')&&(RowData[RowCounter][2]=='n')))
    {
        /*  HexDecBin Parametr !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         Hex == 0
         Dec == 1
         DecSign == 2
         Bin == 3
         !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         */
        //-----------8---------------
        if(RowParamStruct[RowCounter].Count8_16_32==8)
        {
            //Dec
            if(RowParamStruct[RowCounter].HexDecBinCount==1)
            {
                       i=0;
                       while(RowData[RowCounter][i]!='\0')
                       {
                           if((RowData[RowCounter][i]==' '))
                               {
                               i++;
                               }
                           else
                               {
                               DataPr.clear();
                               DataPr[0]=RowData[RowCounter][i];
                               DataPr[1]=RowData[RowCounter][i+1];
                               DataPr2=(DataPr).toUInt(&ok,16);
                               DataPr.clear();
                                i=i+2;
                               }
                          }
                       RowData[RowCounter].clear();
                       RowData[RowCounter].append(QString().sprintf("%-3u",DataPr2));
                }
                  //DecSign
                  if(RowParamStruct[RowCounter].HexDecBinCount==2)
                  {
                       i=0;
                       while(RowData[RowCounter][i]!='\0')
                       {
                           if((RowData[RowCounter][i]==' '))
                               {
                               i++;
                               }
                           else
                               {
                               DataPr.clear();
                               DataPr[0]=RowData[RowCounter][i];
                               DataPr[1]=RowData[RowCounter][i+1];
                               DataPr2=(DataPr).toUInt(&ok,16);
                               DataPr.clear();
                                i=i+2;
                               }
                          }
                       RowData[RowCounter].clear();
                       RowData[RowCounter].append(QString().sprintf("%-4d",(signed char)DataPr2));
                    }
                  //bin
                  if(RowParamStruct[RowCounter].HexDecBinCount==3)
                  {
                       i=0;
                       while(RowData[RowCounter][i]!='\0')
                       {
                           if((RowData[RowCounter][i]==' '))
                               {
                               i++;
                               }
                           else
                               {
                               DataPr.clear();
                               DataPr[0]=RowData[RowCounter][i];
                               DataPr[1]=RowData[RowCounter][i+1];
                               DataPr2=(DataPr).toUInt(&ok,16);
                               itoa(DataPr2,DataPr3,2);
                               AddZero=QString(DataPr3);
                               ZeroSize=AddZero.size();
                               for(int a=0;a<(8-ZeroSize);a++)
                               {
                                   AddZero.insert(0,"0");
                               }
                               DataBit.append(AddZero);
                               DataPr.clear();
                                i=i+2;
                               }
                          }
                       RowData[RowCounter].clear();
                       RowData[RowCounter].append(DataBit);
                       DataBit.clear();
                   }
            }
            //----------------16----------------
            if(RowParamStruct[RowCounter].Count8_16_32==16)
            {
                //Dec
                if(RowParamStruct[RowCounter].HexDecBinCount==1)
                {
                           i=0;
                           while(RowData[RowCounter][i]!='\0')
                           {
                               if((RowData[RowCounter][i]==' '))
                                   {
                                   i++;
                                   }
                               else
                                   {
                                   DataPr.clear();
                                   DataPr[0]=RowData[RowCounter][i];
                                   DataPr[1]=RowData[RowCounter][i+1];
                                   DataPr[2]=RowData[RowCounter][i+2];
                                   DataPr[3]=RowData[RowCounter][i+3];
                                   DataPr2=(DataPr).toUInt(&ok,16);
                                   DataPr.clear();
                                    i=i+4;
                                   }
                              }
                           RowData[RowCounter].clear();
                           RowData[RowCounter].append(QString().sprintf("%-5u",DataPr2));
                    }

                //float
                try
                {
                    if(RowParamStruct[RowCounter].HexDecBinCount==4)
                    {
                               i=0;
                               while(RowData[RowCounter][i]!='\0')
                               {
                                   if((RowData[RowCounter][i]==' '))
                                       {
                                       i++;
                                       }
                                   else
                                       {
                                       DataPr.clear();
                                       DataPr[0]=RowData[RowCounter][i];
                                       DataPr[1]=RowData[RowCounter][i+1];
                                       DataPr[2]=RowData[RowCounter][i+2];
                                       DataPr[3]=RowData[RowCounter][i+3];
                                       DataPr2=(DataPr).toInt(&ok,16);
                                       FloatData =(float*)(&DataPr2);
                                       DataPr.clear();
                                        i=i+4;
                                       }
                                  }
                               RowData[RowCounter].clear();
                               RowData[RowCounter].append(QString().sprintf("%g",*FloatData));
                        }
                }
                catch(...)
                {
                    qDebug()<<"Float reading err";
                }
                //DecSign
                if(RowParamStruct[RowCounter].HexDecBinCount==2)
                {
                     i=0;
                     while(RowData[RowCounter][i]!='\0')
                     {
                         if((RowData[RowCounter][i]==' '))
                             {
                             i++;
                             }
                         else
                             {
                             DataPr.clear();
                             DataPr[0]=RowData[RowCounter][i];
                             DataPr[1]=RowData[RowCounter][i+1];
                             DataPr[2]=RowData[RowCounter][i+2];
                             DataPr[3]=RowData[RowCounter][i+3];
                             DataPr2=(DataPr).toUInt(&ok,16);
                             DataPr.clear();
                              i=i+4;
                             }
                        }
                     RowData[RowCounter].clear();
                     RowData[RowCounter].append(QString().sprintf("%-6d",(signed short)DataPr2));
                  }
                //bit
                if(RowParamStruct[RowCounter].HexDecBinCount==3)
                {
                     i=0;
                     while(RowData[RowCounter][i]!='\0')
                     {
                         if((RowData[RowCounter][i]==' '))
                             {
                             i++;
                             }
                         else
                             {
                             DataPr.clear();
                             DataPr[0]=RowData[RowCounter][i];
                             DataPr[1]=RowData[RowCounter][i+1];
                             DataPr[2]=RowData[RowCounter][i+2];
                             DataPr[3]=RowData[RowCounter][i+3];
                             DataPr2=(DataPr).toUInt(&ok,16);
                             itoa(DataPr2,DataPr3,2);
                             AddZero=QString(DataPr3);
                             ZeroSize=AddZero.size();
                             for(int a=0;a<(16-ZeroSize);a++)
                             {
                                 AddZero.insert(0,"0");
                             }
                             DataBit.append(AddZero);
                             DataPr.clear();
                              i=i+4;
                             }
                        }
                     RowData[RowCounter].clear();
                     RowData[RowCounter].append(DataBit);
                     DataBit.clear();
                 }
            }
            //--------------------------32------------------------------
            if(RowParamStruct[RowCounter].Count8_16_32==32)
            {

                //Dec
                if(RowParamStruct[RowCounter].HexDecBinCount==1)
                {
                           i=0;
                           while(RowData[RowCounter][i]!='\0')
                           {
                               if((RowData[RowCounter][i]==' '))
                                   {
                                   i++;
                                   }
                               else
                                   {
                                   DataPr.clear();
                                   DataPr[0]=RowData[RowCounter][i];
                                   DataPr[1]=RowData[RowCounter][i+1];
                                   DataPr[2]=RowData[RowCounter][i+2];
                                   DataPr[3]=RowData[RowCounter][i+3];
                                   DataPr[4]=RowData[RowCounter][i+4];
                                   DataPr[5]=RowData[RowCounter][i+5];
                                   DataPr[6]=RowData[RowCounter][i+6];
                                   DataPr[7]=RowData[RowCounter][i+7];

                                   DataPr2=(DataPr).toUInt(&ok,16);
                                   DataPr.clear();
                                    i=i+8;
                                   }
                              }
                           RowData[RowCounter].clear();
                           RowData[RowCounter].append(QString().sprintf("%-10u",DataPr2));
                    }


                // Float
                try
                {
                    if(RowParamStruct[RowCounter].HexDecBinCount==4)
                    {
                               i=0;
                               while(RowData[RowCounter][i]!='\0')
                               {
                                   if((RowData[RowCounter][i]==' '))
                                       {
                                       i++;
                                       }
                                   else
                                       {
                                           DataPr.clear();
                                           DataPr[0]=RowData[RowCounter][i];
                                           DataPr[1]=RowData[RowCounter][i+1];
                                           DataPr[2]=RowData[RowCounter][i+2];
                                           DataPr[3]=RowData[RowCounter][i+3];
                                           DataPr[4]=RowData[RowCounter][i+4];
                                           DataPr[5]=RowData[RowCounter][i+5];
                                           DataPr[6]=RowData[RowCounter][i+6];
                                           DataPr[7]=RowData[RowCounter][i+7];

                                           DataPr2=(DataPr).toInt(&ok,16);
                                           FloatData =(float*)(&DataPr2);
                                           DataPr.clear();
                                           i=i+8;
                                       }
                                  }
                               RowData[RowCounter].clear();
                               RowData[RowCounter].append(QString().sprintf("%g",*FloatData));
                        }
                }
                catch(...)
                {
                    qDebug()<<"Float reading err";
                }

                    //DecSign
                    if(RowParamStruct[RowCounter].HexDecBinCount==2)
                    {
                         i=0;
                         while(RowData[RowCounter][i]!='\0')
                         {
                             if((RowData[RowCounter][i]==' '))
                                 {
                                 i++;
                                 }
                             else
                                 {
                                 DataPr.clear();
                                 DataPr[0]=RowData[RowCounter][i];
                                 DataPr[1]=RowData[RowCounter][i+1];
                                 DataPr[2]=RowData[RowCounter][i+2];
                                 DataPr[3]=RowData[RowCounter][i+3];
                                 DataPr[4]=RowData[RowCounter][i+4];
                                 DataPr[5]=RowData[RowCounter][i+5];
                                 DataPr[6]=RowData[RowCounter][i+6];
                                 DataPr[7]=RowData[RowCounter][i+7];
                                 DataPr2=(DataPr).toUInt(&ok,16);
                                 DataPr.clear();
                                  i=i+8;
                                 }
                            }
                         RowData[RowCounter].clear();
                         RowData[RowCounter].append(QString().sprintf("%-11d",(signed int)DataPr2));
                    }
                    //bit
                    if(RowParamStruct[RowCounter].HexDecBinCount==3)
                    {
                         i=0;
                         while(RowData[RowCounter][i]!='\0')
                         {
                             if((RowData[RowCounter][i]==' '))
                                 {
                                 i++;
                                 }
                             else
                                 {
                                 DataPr.clear();
                                 DataPr[0]=RowData[RowCounter][i];
                                 DataPr[1]=RowData[RowCounter][i+1];
                                 DataPr[2]=RowData[RowCounter][i+2];
                                 DataPr[3]=RowData[RowCounter][i+3];
                                 DataPr[4]=RowData[RowCounter][i+4];
                                 DataPr[5]=RowData[RowCounter][i+5];
                                 DataPr[6]=RowData[RowCounter][i+6];
                                 DataPr[7]=RowData[RowCounter][i+7];
                                 DataPr2=(DataPr).toUInt(&ok,16);
                                 itoa(DataPr2,DataPr3,2);
                                 AddZero=QString(DataPr3);
                                 ZeroSize=AddZero.size();
                                 for(int a=0;a<(32-ZeroSize);a++)
                                 {
                                     AddZero.insert(0,"0");
                                 }
                                 DataBit.append(AddZero);
                                 DataPr.clear();
                                  i=i+8;
                                 }
                            }
                         RowData[RowCounter].clear();
                         RowData[RowCounter].append(DataBit);
                         DataBit.clear();
                     }
            }

             RowCounter++;
        }

    SocketData.clear();
    /*int Proverochka = 0  ;
     while(!(RowData[Proverochka].size()==0))
    {
        qDebug()<<RowData[Proverochka];
        Proverochka++;
    }*/
}

void CellList1::InsertInfoIntoCell(int ColumnCounter)
{
    /*  HexDecBin Parametr !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     Hex == 0
     Dec == 1
     DecSign == 2
     Bin == 3
     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     */
    //-----------Resize column----------
    //Останавливаем сигналы которые реагируют на изменение таблицы
    bool old_blocked = ui->Table->blockSignals(true);
            int ControlWidth=2;
            int CheckCount=0;
            for(int ResizeCount=1;ResizeCount<ui->Table->rowCount();ResizeCount++)
            {
                if(RowParamStruct[ResizeCount-1].Count8_16_32==8)
                {
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==0) CheckCount=10;
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==1) CheckCount=10;
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==2) CheckCount=10;
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==3) CheckCount=10;
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==4) CheckCount=10;
                }
                if(RowParamStruct[ResizeCount-1].Count8_16_32==16)
                {
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==0) CheckCount=10;
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==1) CheckCount=10;
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==2) CheckCount=10;
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==3) CheckCount=15;
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==4) CheckCount=15;
                }
                if(RowParamStruct[ResizeCount-1].Count8_16_32==32)
                {
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==0) CheckCount=10;
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==1) CheckCount=15;
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==2) CheckCount=15;
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==3) CheckCount=27;
                    if(RowParamStruct[ResizeCount-1].HexDecBinCount==4) CheckCount=15;
                }
                if(ControlWidth<=CheckCount) ControlWidth=CheckCount;
            }
            ControlWidth=ControlWidth*8;
            for(int ColumnResizeCount=1;ColumnResizeCount<ui->Table->columnCount();ColumnResizeCount++)
            {
                ui->Table->setColumnWidth(ColumnResizeCount,ControlWidth);
            }

             //Insert info from RowData[n] to column
            for(int ColCount=1;ColCount<ui->Table->rowCount();ColCount++)
            {
                int PaintRed=0;
                //ui->Table->item(ColCount,ColumnCounter+1)->setText("");
                //Если стоит прочерк отмечаем красным
                if(ui->Table->item(ColCount,ColumnCounter+1)->text()=="-"){
                    ui->Table->item(ColCount,ColumnCounter+1)->setBackgroundColor(Qt::red);
                    //Если отмечать ячейки памяти  в который записано "-" поставить ниже PaintRed=1;
                    PaintRed=0;
                }
                else
                {
                    //Если записанно как конец аналогично
                    if(RowData[ColCount-1]=="fin"){ui->Table->item(ColCount,ColumnCounter+1)->setBackgroundColor(Qt::red); ui->Table->item(ColCount,ColumnCounter+1)->setText("empty");}
                    else ui->Table->item(ColCount,ColumnCounter+1)->setText(RowData[ColCount-1]);
                }
                if(RowData[ColCount-1].size()==0)
                {
                    ui->Table->item(ColCount,ColumnCounter+1)->setBackgroundColor(Qt::red);
                    if(!(ui->Table->item(ColCount,ColumnCounter+1)->text()=="-"))
                    ui->Table->item(ColCount,ColumnCounter+1)->setText("empty");
                }
                else
                {
                    if((PaintRed==1)) ui->Table->item(ColCount,ColumnCounter+1)->setBackgroundColor(Qt::red);
                    else ui->Table->item(ColCount,ColumnCounter+1)->setBackgroundColor(Qt::white);
                }
                PaintRed=0;
            }
            //Clear RowData[]
    for(int i=0;i<RowParamStructCounter;i++)
    {
    RowData[i].clear();
    }
    //Включаем сигналы, которые реагируют на изменение таблицы
    ui->Table->blockSignals(old_blocked);
    //ui->Table->resizeColumnsToContents();
}

void CellList1::closeEvent(QCloseEvent *e)
{
    Timer->stop();
    delete ui;
}

void RowSettings::closeEvent(QCloseEvent *e)
{
    emit send_to_cellList();
}

void CellList1::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Delete)
    {
        if(ui->Table->rowCount()>2)
        {
        ui->Table->removeRow(ui->Table->selectionModel()->currentIndex().row());
        }
    }
}
void CellList1::GetAddressParamList()
{
    QLineEdit* CurrentLine = (QLineEdit*)sender();
    QStringList CurrentNameList;
    QString text=CurrentLine->text();
    dbal_getsuggestedparamlist(text,&CurrentNameList);
    QCompleter* Completer = new QCompleter(CurrentNameList,this);
    QString Element;
    int MaxElementWidth=0;
    for(int i=0;i<CurrentNameList.size();i++)
    {
        Element=CurrentNameList[i];
        if(Element.size()>MaxElementWidth)
            MaxElementWidth=Element.size();
    }
    Completer->popup()->setFixedWidth(Completer->popup()->fontMetrics().width('O')*MaxElementWidth*1.2);
    Completer->setCaseSensitivity(Qt::CaseInsensitive);
    CurrentLine->setCompleter(Completer);
}

void CellList1::HaltResumeStateCheck(int CurrentColumn)
{
    if(CellSocket->state()==QAbstractSocket::ConnectedState)
    {
        Timer->stop();
        CellSocketData.clear();
        QString TargetName;
        //Получаем имя устройства
        CellSocket->write(QString().sprintf("capture \"target current\"\x1A"));
        HaltResStateChRead();
        CellSocketData.replace(" ","");
        CellSocketData.replace("\x1A","");
        TargetName=CellSocketData;
        CellSocketData.clear();
        //Проверяем его состояние
        CellSocket->write(QString("capture \"%0 curstate\"\x1A").arg(TargetName));
        HaltResStateChRead();
        CellSocketData.replace(" ","");
        CellSocketData.replace("\x1A","");
        QComboBox *CurrentBox = new QComboBox();
        CurrentBox = qobject_cast<QComboBox*>(ui->Table->cellWidget(0,CurrentColumn+1));
        if(CellSocketData=="halted")
        {          
            CurrentBox->setBackgroundColor(Qt::yellow);
        }
        if(CellSocketData=="running")
        {
            CurrentBox->setBackgroundColor(Qt::cyan);
        }
        CellSocketData.clear();
        Timer->start();
    }
}
void CellList1::HaltResStateChRead()
{
    bool exitCounter=false;
    int iter=0;
    QByteArray SocReadInfo;
    while(CellSocketData.size()<=20000)
    {
        //CellSocket->waitForReadyRead(100);
        //Ждем 5 сек и отключаемся если socket не отвечает
        if(CellSocket->waitForReadyRead(5000)){}
        else
        {
            CellSocketData.clear();
            CellSocket->disconnectFromHost();
            break;
        }
        //Reading
        SocReadInfo=CellSocket->peek(CellSocket->bytesAvailable());
        for(iter=0;iter<SocReadInfo.size();iter++)
        {
            if(SocReadInfo[iter]=='\x1A')
            {
                exitCounter=true;
                CellSocketData.append(CellSocket->read(iter=iter+1));
                break;
            }
        }
        SocReadInfo.clear();
        if(exitCounter==true) break;
    }
}

void CellList1::SaveSelectedCellToBuffer()
{
    QString cbStr;
    QClipboard *cb = QApplication::clipboard();
    QModelIndexList list = ui->Table->selectionModel()->selectedIndexes();
    int i,j, firstRow, lastRow, rowCount;
    if(list.isEmpty()) return;

    firstRow = list.first().row() ;
    lastRow = list.last().row();
    rowCount = lastRow-firstRow+1;

    for(i=0;i<rowCount;++i,cbStr+='\n')
        for(j=i;j<list.count();j+=rowCount,cbStr+='\t')
            cbStr+=ui->Table->model()->data(list[j],Qt::EditRole).toString();
    cb->setText(cbStr);
}
