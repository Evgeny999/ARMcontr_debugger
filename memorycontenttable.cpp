#include "memorycontenttable.h"
#include "ui_memorycontenttable.h"


MemoryContentTable::MemoryContentTable(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MemoryContentTable)
{
    ui->setupUi(this);
    MemoryContentSocket = new QTcpSocket(this);

    MemoryContentTimer = new QTimer(this);
    MemoryContentTimerInterval = 500;
    SetTimerInterval();
    connect(MemoryContentTimer,SIGNAL(timeout()),this,SLOT(StartWorking()));

    GetParamToComboBox();
    ui->FormatComboBox->addItem("8");
    ui->FormatComboBox->addItem("16");
    ui->FormatComboBox->addItem("32");
    ui->HexDecBinComboBox->addItem("hex");
    ui->HexDecBinComboBox->addItem("dec");
    ui->HexDecBinComboBox->addItem("decSign");
    ui->HexDecBinComboBox->addItem("bin");
    ui->HexDecBinComboBox->addItem("float");
    ui->FormatComboBox->setCurrentIndex(2);
    ui->AddressLine->setMinimumWidth(ui->AddressLine->fontMetrics().width('0')*12);
    ui->SettingsComboBox->setStyleSheet("padding: 0");

    ui->InfoTable->setRowCount(2);
    ui->InfoTable->setColumnCount(2);
    ui->InfoTable->setVerticalHeaderItem(1,new QTableWidgetItem(tr(QString().sprintf("0x%08X",0))));
    ui->InfoTable->verticalScrollBar()->hide();
    ui->InfoTable->horizontalScrollBar()->hide();

    connect(ui->AddressLine,SIGNAL(textEdited(QString)),this,SLOT(GetAddressParamList()));
    connect(ui->InfoTable,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(InfoTableItemChanged(QTableWidgetItem*)));
    ui->InfoTable->blockSignals(true);
    ui->AddressLine->setText(QString("0x00000010"));
    //ui->AddressLine->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9}_{*]{10}"),this));


    QAction *SaveToBufAction = new QAction("SaveToBuf",this);
    connect(SaveToBufAction,SIGNAL(triggered()),this,SLOT(SaveSelectedCellToBuffer()));
    menuBar()->addAction(SaveToBufAction);

    this->menuBar();
    menuBar()->addAction("|Частота обновления->");
    menuBar()->addSeparator();
     menuBar()->addSeparator();
    QAction *Action_05_sec = new QAction("0.5sec",this);
    connect(Action_05_sec,SIGNAL(triggered()),this,SLOT(SetDelay05Sec()));
    menuBar()->addAction(Action_05_sec);
    QAction *Action_1_sec = new QAction("1sec",this);
    connect(Action_1_sec,SIGNAL(triggered()),this,SLOT(SetDelay1Sec()));
    menuBar()->addAction(Action_1_sec);
    QAction *Action_3_sec = new QAction("3sec",this);
    connect(Action_3_sec,SIGNAL(triggered()),this,SLOT(SetDelay3Sec()));
    menuBar()->addAction(Action_3_sec);
    QAction *Action_5_sec = new QAction("5sec |",this);
    connect(Action_5_sec,SIGNAL(triggered()),this,SLOT(SetDelay5Sec()));
    menuBar()->addAction(Action_5_sec);

    int ComboBoWidth = ui->SettingsComboBox->fontMetrics().width('0')*10;
    ui->SettingsComboBox->setFixedWidth(ComboBoWidth);


    GetFormat();
    SetStartTableFormat();

}

MemoryContentTable::~MemoryContentTable()
{
    delete ui;
}

void MemoryContentTable::SetDelay05Sec()
{
    MemoryContentTimerInterval = 500;
    SetTimerInterval();
}
void MemoryContentTable::SetDelay1Sec()
{
    MemoryContentTimerInterval = 1000;
    SetTimerInterval();
}
void MemoryContentTable::SetDelay3Sec()
{
    MemoryContentTimerInterval = 3000;
    SetTimerInterval();
}
void MemoryContentTable::SetDelay5Sec()
{
    MemoryContentTimerInterval = 5000;
    SetTimerInterval();
}
void MemoryContentTable::SetTimerInterval()
{
    MemoryContentTimer->setInterval(MemoryContentTimerInterval);
}


void MemoryContentTable::TimerStop()
{
    MemoryContentTimer->stop();
}

void MemoryContentTable::StartWorking()
{
    GetFormat();
    GetSizes();
    FillTableHeaders();
    SearchSocket();
    SockedCheackState();
}
void MemoryContentTable::InfoTableItemChanged(QTableWidgetItem *ChItem)
{
    MemoryContentTimer->stop();
    ui->InfoTable->blockSignals(true);
    int ItemColumn = ChItem->column();
    int ItemRow = ChItem->row();
    QString WriteValue = ChItem->text();
    WriteValue.replace(" ","");
    signed char Char=0;
    signed short Short=0;
    signed int Int=0;
    int WriteValue_int=0;
    bool ok;

    float WriteValue_float = 0;

    int WordCounter=1;
    if(F8_16_32Param==0)
        WordCounter=1;
    if(F8_16_32Param==1)
        WordCounter=2;
    if(F8_16_32Param==2)
        WordCounter=4;
    QString ItemRowInfo=ui->InfoTable->verticalHeaderItem(ItemRow)->text();
    ItemRowInfo.replace(":","").replace(" ","");
    int WriteAdd_int = ItemRowInfo.toInt(&ok,16) + ItemColumn*WordCounter;


    if(HEXDECBINParam==0)
    {
     WriteValue_int = WriteValue.toUInt(&ok,16);
    }
    if(HEXDECBINParam==1)
    {
      WriteValue_int = WriteValue.toInt(&ok,10);
    }
    if(HEXDECBINParam==2)
    {
        Char=WriteValue.toInt(&ok,10);
        Short=WriteValue.toInt(&ok,10);
        Int=WriteValue.toInt(&ok,10);
    }
    if(HEXDECBINParam==3)
    {
      WriteValue_int = WriteValue.toUInt(&ok,2);
    }
    if(HEXDECBINParam==4)
    {
      WriteValue_float = WriteValue.toFloat(&ok);
      int *float_to_int = (int*)(&WriteValue_float);
      WriteValue_int = *float_to_int ;
    }


    if(HEXDECBINParam==0)//IF hex
    {
             if(F8_16_32Param==0)
             {
                 if(WriteValue.size()<=2)
                 MemoryContentSocket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
             }
             if(F8_16_32Param==1)
             {
                 if(WriteValue.size()<=4)
                 MemoryContentSocket->write(QString().sprintf("capture \"mwh 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
             }
             if(F8_16_32Param==2)
             {
                 if(WriteValue.size()<=8)
                 MemoryContentSocket->write(QString().sprintf("capture \"mww 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
             }
     }
     else if((HEXDECBINParam==1))//if dec
     {
         qDebug()<<"111";
             if(F8_16_32Param==0)
             {
                 if(WriteValue.size()<=3)
                 MemoryContentSocket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
             }
             if(F8_16_32Param==1)
             {
                 if(WriteValue.size()<=5)
                 MemoryContentSocket->write(QString().sprintf("capture \"mwh 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
             }
             if(F8_16_32Param==2)
             {
                 if(WriteValue.size()<=10)
                 MemoryContentSocket->write(QString().sprintf("capture \"mww 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
             }
     }
     else if(HEXDECBINParam== 2)//if dec sign
     {
         if(F8_16_32Param==0)
         {
             if(WriteValue.size()<=8)
             MemoryContentSocket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",WriteAdd_int,Char));
         }
         if(F8_16_32Param==1)
         {
             if(WriteValue.size()<=16)
             MemoryContentSocket->write(QString().sprintf("capture \"mwh 0x%X 0x%X\"\x1A",WriteAdd_int,Short));
         }
         if(F8_16_32Param==2)
         {
             if(WriteValue.size()<=32)
             MemoryContentSocket->write(QString().sprintf("capture \"mww 0x%X 0x%X\"\x1A",WriteAdd_int,Int));
         }
     }
     else if(HEXDECBINParam==3)//if bin
     {
         if(F8_16_32Param==0)
         {
             if(WriteValue.size()<=8)
             MemoryContentSocket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
         }
         if(F8_16_32Param==1)
         {
             if(WriteValue.size()<=16)
             MemoryContentSocket->write(QString().sprintf("capture \"mwh 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
         }
         if(F8_16_32Param==2)
         {
             if(WriteValue.size()<=32)
             MemoryContentSocket->write(QString().sprintf("capture \"mww 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
         }
     }
     else if(HEXDECBINParam==4)//if float
     {
             if(F8_16_32Param==0)
             {
                 //if(WriteValue.size()<=3)
                 MemoryContentSocket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
             }
             if(F8_16_32Param==1)
             {
                 //if(WriteValue.size()<=5)
                 MemoryContentSocket->write(QString().sprintf("capture \"mwh 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
             }
             if(F8_16_32Param==2)
             {
                 //if(WriteValue.size()<=10)
                 MemoryContentSocket->write(QString().sprintf("capture \"mww 0x%X 0x%X\"\x1A",WriteAdd_int,WriteValue_int));
             }
     }

    ui->InfoTable->blockSignals(false);
    ReadFromSocket();
    MemoryContentTimer->start();
}

void MemoryContentTable::GetFormat()
{
    /*
     0-hex
     1-dec
     2-decsign
     3-bin
     -----
     0-8
     1-16
     2-32
     */
     F8_16_32Param  = ui->FormatComboBox->currentIndex();
     HEXDECBINParam = ui->HexDecBinComboBox->currentIndex();

     if(F8_16_32Param == 0) //8
     {
         if(HEXDECBINParam == 0){FormatCounter = 20; Kformat=3;} //hex
         if(HEXDECBINParam == 1){FormatCounter = 4; Kformat=4;} //dec
         if(HEXDECBINParam == 2){FormatCounter = 9; Kformat = 5;} //decsign
         if(HEXDECBINParam == 3){FormatCounter = 3; Kformat=9;} //bin
         if(HEXDECBINParam == 4){FormatCounter = 55; Kformat=16;} //float
     }
     if(F8_16_32Param == 1) //16
     {
         if(HEXDECBINParam == 0){FormatCounter = 21; Kformat = 5;} //hex
         if(HEXDECBINParam == 1){FormatCounter = 6; Kformat = 6;} //dec
         if(HEXDECBINParam == 2){FormatCounter = 10; Kformat = 7;} //decsign
         if(HEXDECBINParam == 3){FormatCounter = 5; Kformat = 17;} //bin
         if(HEXDECBINParam == 4){FormatCounter = 56; Kformat=16;} //float
     }
     if(F8_16_32Param == 2) //32
     {
         if(HEXDECBINParam == 0){FormatCounter = 22; Kformat = 9;} //hex
         if(HEXDECBINParam == 1){FormatCounter = 8; Kformat = 11;} //dec
         if(HEXDECBINParam == 2){FormatCounter = 11; Kformat = 12;} //decsign
         if(HEXDECBINParam == 3){FormatCounter = 7; Kformat = 33;} //bin
         if(HEXDECBINParam == 4){FormatCounter = 57; Kformat=16;} //float
     }

}

void MemoryContentTable::GetSizes()
{
    int TableWorkspaceWidth=ui->InfoTable->width()-ui->InfoTable->verticalHeader()->width();
    int TableWorkspaceHeight=ui->InfoTable->height();
    int ElemWidth = ui->InfoTable->fontMetrics().width('0');
    int ElemHeight = ui->InfoTable->fontMetrics().height();
    float WidthEl = TableWorkspaceWidth/((Kformat+1)*ElemWidth);
    WidthElements = floor(WidthEl);
    float HeightEl = TableWorkspaceHeight/(ElemHeight*2);
    HeightElements = floor(HeightEl);

    int HeightElementDifference = HeightEl-ui->InfoTable->rowCount()-1;
    int WidthElementDifference = WidthEl-ui->InfoTable->columnCount();

    if(HeightElementDifference>0)
    {
        for(int i=0;i<HeightElementDifference;i++)
        {
            AddRow();
        }
    }
    else
    {
        for(int i=0;i>HeightElementDifference;i--)
        {
            if(ui->InfoTable->rowCount()>1)
            ui->InfoTable->removeRow(ui->InfoTable->rowCount()-1);
        }
    }

    if(WidthElementDifference>0)
    {
        for(int i=0;i<WidthElementDifference;i++)
        {
            AddColumn();
        }
    }
    else
    {
        for(int i=0;i>WidthElementDifference;i--)
        {
            if(ui->InfoTable->columnCount()>1)
            ui->InfoTable->removeColumn(ui->InfoTable->columnCount()-1);
        }
    }

    for(int CollCount=0;CollCount<ui->InfoTable->columnCount();CollCount++)
    {
        ui->InfoTable->setColumnWidth(CollCount,(Kformat+1)*ElemWidth);
    }
    for(int RowCount=0;RowCount<ui->InfoTable->columnCount();RowCount++)
    {
        ui->InfoTable->setRowHeight(RowCount,ElemHeight*2);
    }
}

void MemoryContentTable::SetStartTableFormat()
{
    int OneCellWidth = ui->InfoTable->fontMetrics().width('0')*(Kformat+1);
    int StartWindowWidth=OneCellWidth*5+ui->InfoTable->verticalHeader()->width();
    this->setGeometry(this->x(),this->y(),StartWindowWidth,this->height());
}

void MemoryContentTable::ReadFromSocket()
{
    bool exitCounter=false;
    int iter=0;
    QByteArray SocReadInfo;
    while(MemoryContentData.size()<=20000000)
    {
        //CellSocket->waitForReadyRead(100);
        //Ждем 5 сек и отключаемся если socket не отвечает
        if(MemoryContentSocket->waitForReadyRead(5000)){}
        else
        {
            MemoryContentData.clear();
            MemoryContentSocket->disconnectFromHost();
            break;
        }
        //Reading
        SocReadInfo=MemoryContentSocket->peek(MemoryContentSocket->bytesAvailable());
        for(iter=0;iter<SocReadInfo.size();iter++)
        {
            if(SocReadInfo[iter]=='\x1A')
            {
                exitCounter=true;
                MemoryContentData.append(MemoryContentSocket->read(iter=iter+1));
                break;
            }
        }
        SocReadInfo.clear();
        if(exitCounter==true) break;
    }
}

void MemoryContentTable::ChannelCheck()
{
    bool CheckSearchChannel=false;
    bool StartToReWrite=false;
    bool ok;
    QString CurrentChannel;
    int CurrentChannelIterator=0;
    int ProcessWindowDataIterator=0;
    if(NumberOfChannelControl>1)
    {
        MemoryContentData.clear();
        MemoryContentSocket->write(QString().sprintf("capture \"pcilink_setchan %d\"\x1A",CHANNEL));
        ReadFromSocket();
        MemoryContentData.replace(" ","");
        MemoryContentData.replace("\x1A","");
        while(MemoryContentData[ProcessWindowDataIterator]!='\0')
        {
            if(StartToReWrite==true)
            {
                CurrentChannel[CurrentChannelIterator]=MemoryContentData[ProcessWindowDataIterator];
                CurrentChannelIterator++;
            }
            if(MemoryContentData[ProcessWindowDataIterator]=='=') StartToReWrite=true;
            ProcessWindowDataIterator++;
        }
        // использую повторно CurrentChannelIterator не как итератор
        CurrentChannelIterator=CurrentChannel.toInt(&ok,10);
        if(CurrentChannelIterator==CHANNEL) CheckSearchChannel=true;
    }else CheckSearchChannel=true;
    MemoryContentData.clear();
}

void MemoryContentTable::FillTableHeaders()
{
    ui->InfoTable->blockSignals(true);
    QTableWidgetItem *HorizHeadItem;
    QTableWidgetItem *VerticalHeadItem;
    int F8_16_32 = F8_16_32Param;
    int WordCounter=1;
    if(F8_16_32==0)
        WordCounter=1;
    if(F8_16_32==1)
        WordCounter=2;
    if(F8_16_32==2)
        WordCounter=4;
    for(int i=0;i<ui->InfoTable->columnCount();i++)
    {
        HorizHeadItem= new QTableWidgetItem(QString().sprintf("%X",i*WordCounter));
        ui->InfoTable->setHorizontalHeaderItem(i,HorizHeadItem);
    }

    bool ok;
    QString Address;
    Address=ui->AddressLine->text();
    if((Address[0]=='0')&&(Address[1]=='x'))
    {
        AddressLineInfo=ui->AddressLine->text().toInt(&ok,16);
    }
    else
    {
       bool update=false;
       varlistelm_t *RecombAddress = new varlistelm_t;
       dbal_fillfromdb(Address,RecombAddress,update);
       AddressLineInfo=RecombAddress->addr;
    }

    int CurrentAddrInsertIntoVertHeader=AddressLineInfo;
    int columnNumber=ui->InfoTable->columnCount();
    for(int i=0;i<ui->InfoTable->rowCount();i++)
    {
        VerticalHeadItem = new QTableWidgetItem(QString().sprintf("%08X :",CurrentAddrInsertIntoVertHeader+i*WordCounter*columnNumber));
        ui->InfoTable->setVerticalHeaderItem(i,VerticalHeadItem);
    }
    ui->InfoTable->blockSignals(false);
}

void MemoryContentTable::AddRow()
{
    ui->InfoTable->blockSignals(true);
        QTableWidgetItem *TableItem;
        ui->InfoTable->insertRow(ui->InfoTable->rowCount());
        for(int colcount=0;colcount<ui->InfoTable->columnCount();colcount++)
        {
            for(int rowcount=0;rowcount<ui->InfoTable->rowCount();rowcount++)
            {
                if(!(ui->InfoTable->item(rowcount,colcount)))
                {
                    TableItem = new QTableWidgetItem("value");
                    TableItem->setBackgroundColor(Qt::white);
                    ui->InfoTable->setItem(rowcount,colcount,TableItem);
                }
             }
        }
        ui->InfoTable->blockSignals(false);
}

void MemoryContentTable::SearchSocket()
{
    int QuantitiIterator=0;
    while(!Quantiti[QuantitiIterator].Flag==0)
    {
        if((IPADD==Quantiti[QuantitiIterator].IpAddr)&&(PORT==Quantiti[QuantitiIterator].Port))
        {
            MemoryContentSocket=&(Quantiti[QuantitiIterator].IpPortSocket);
            NumberOfChannelControl=Quantiti[QuantitiIterator].NumberOfChannal;
        }
        QuantitiIterator++;
    }
}

void MemoryContentTable::SockedCheackState()
{
    switch(MemoryContentSocket->state())
    {
        case (QAbstractSocket::ConnectedState):
        {
            ui->InfoTable->blockSignals(false);
            ChannelCheck();
            SendComand();
            ReadFromSocket();
            Filter();
            FillTable();
            break;
        }
        case(QAbstractSocket::UnconnectedState):
        {
            MemoryContentSocket->connectToHost(QString("%0").arg(IPADD),PORT);
            break;
        }
        case(QAbstractSocket::ConnectingState):
        {
             break;
        }
    default:
        break;
    }
}

void MemoryContentTable::SendComand()
{
    if(MemoryContentSocket->state()==QAbstractSocket::ConnectedState)
    {
        int number=WidthElements*HeightElements;
        if (F8_16_32Param == 0)
            {
              MemoryContentSocket->write(QString().sprintf("capture \"mdb 0x%X %d\"\x1A",AddressLineInfo,number));
            }
        if (F8_16_32Param == 1)
            {
              MemoryContentSocket->write(QString().sprintf("capture \"mdh 0x%X %d\"\x1A",AddressLineInfo,number));
            }
        if (F8_16_32Param == 2)
            {
              MemoryContentSocket->write(QString().sprintf("capture \"mdw 0x%X %d\"\x1A",AddressLineInfo,number));
            }
    }
}

void MemoryContentTable::AddColumn()
{
    ui->InfoTable->blockSignals(true);
    QTableWidgetItem *TableItem;
    ui->InfoTable->insertColumn(ui->InfoTable->columnCount());
    for(int colcount=0;colcount<ui->InfoTable->columnCount();colcount++)
    {
        for(int rowcount=0;rowcount<ui->InfoTable->rowCount();rowcount++)
        {
            if(!(ui->InfoTable->item(rowcount,colcount)))
            {
                TableItem = new QTableWidgetItem("value");
                TableItem->setBackgroundColor(Qt::white);
                ui->InfoTable->setItem(rowcount,colcount,TableItem);
            }
         }
    }
    ui->InfoTable->blockSignals(false);
}


void MemoryContentTable::GetParamToComboBox()
{
        NameMass->clear();
        int i=0;
        int NameCounter=0;
        int NumOfCh=0;
        int CostomNameIterator=0;
        int CostomNameMassIterator=0;
        int j=0;
        QString Name;
        QString CostomNameMass[MaxNumberOfCostomName];
        ui->SettingsComboBox->addItem("ConnectionSetting");
        ui->SettingsComboBox->addItem("____________________________________");
        while(Quantiti[i].Flag!=0)
        {
            NumOfCh=Quantiti[i].NumberOfChannal;
            //AddCostomName
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
                Name=QString(" IP:%0    PORT:%1    Channel:%2     Name:%4 ").arg(Quantiti[i].IpAddr).arg(QString::number(Quantiti[i].Port)).arg(QString::number(h)).arg(CostomNameMass[h]);
                NameMass[NameCounter]=Name;
                NymberOfChConrolMass[NameCounter]=Quantiti[i].NumberOfChannal;
                ui->SettingsComboBox->addItem(Name,QVariant(NameCounter));
                NameCounter++;
            }
            i++;
            ui->SettingsComboBox->addItem("------------------------------------------------------");
        }
        qobject_cast<QListView*>(ui->SettingsComboBox->view())->setMinimumWidth(300);
        ui->SettingsComboBox->setEditable(true);
        ui->SettingsComboBox->lineEdit()->setReadOnly(true);
        ui->SettingsComboBox->lineEdit()->setAlignment(Qt::AlignRight);
        connect(ui->SettingsComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ComboBoxChangeParam(int)));
}

void MemoryContentTable::ComboBoxChangeParam(int index)
{
    MemoryContentTimer->stop();
    IPADD.clear();
    PORT=0;
    int i=0;
    int C1=0;
    QString Par;
    QString PortChanPr;
    int value=ui->SettingsComboBox->itemData(index).toInt();
    Par=NameMass[value];
    while(Par[i]!='\0')
    {
        if((Par[i]=='I')&&(Par[i+1]=='P')&&(Par[i+2]==':'))
        {
            i=i+3;
            while(Par[i]!=' ')
            {
                IPADD[C1]=Par[i];
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
            CHANNEL=PortChanPr.toInt();
            C1=0;
            PortChanPr.clear();
        }
        i++;
    }

    MemoryContentTimer->start();
}

void MemoryContentTable::Filter()
{
    int NumOfLocalData=0;
    int NumOfDellData=0;
    int LocalCounter=0;
    bool ok;
    QString DataDec;
    QString DataPr;
    QString AddZero;
    int ZeroSize;
    int DataPr2;
    QString DataBit;
    char DataPr3[40];
    int i=0;
    int j=0;

    QString LocalData=QString(MemoryContentData);
    QString DellData;
    MemoryContentData.clear();

    //Filter Intfo
    while(LocalData[NumOfLocalData]!='\0')
    {
        if(LocalData[NumOfLocalData]=='\n')
        {
            LocalCounter=0;
        }

        if(LocalCounter==1)
        {
            DellData[NumOfDellData]=LocalData[NumOfLocalData];
            NumOfDellData++;
        }

        if(LocalData[NumOfLocalData]==':')
        {
            LocalCounter=1;
            NumOfLocalData++;
        }

        NumOfLocalData++;
    }    //qDebug()<<DellData;
    LocalData.clear();
    DellData.replace("\n"," ");
    DellData.replace("\x1A"," ");
    //Set Format Hex Dec Bin
    //Set 8Dec------------------------
     if((FormatCounter == 4)&&(F8_16_32Param==0))
        {
         i=0;
        while(DellData[i]!='\0')
        {
            if((DellData[i]==' '))
                {
                i++;
                }
            else
                {
                DataPr.clear();
                DataPr[0]=DellData[i];
                DataPr[1]=DellData[i+1];
                DataPr2=(DataPr).toUInt(&ok,16);
                DataPr.clear();
                DataDec.append(QString().sprintf("%u",DataPr2)).append(" ");
                DataPr.clear();
                 i=i+2;
                }
          }
          DellData=DataDec;
          DataDec.clear();
         }
     //Set 16Dec
     if((FormatCounter == 6)&&(F8_16_32Param==1))
        {
         i=0;
        while(DellData[i]!='\0')
        {
            if((DellData[i]==' '))
                {
                i++;
                }
            else
                {

                DataPr.clear();
                DataPr[0]=DellData[i];
                DataPr[1]=DellData[i+1];
                DataPr[2]=DellData[i+2];
                DataPr[3]=DellData[i+3];
                DataPr2=(DataPr).toUInt(&ok,16);
                DataPr.clear();
                DataDec.append(QString().sprintf("%u",DataPr2)).append(" ");
                DataPr.clear();
                i=i+4;
                }
            }
          DellData=DataDec;
          DataDec.clear();
         }
     //Set 32Dec
     if((FormatCounter == 8)&&(F8_16_32Param==2))
        {
         i=0;
        while(DellData[i]!='\0')
        {
            if((DellData[i]==' '))
                {
                i++;
                }
            else
                {
                DataPr.clear();
                DataPr[0]=DellData[i];
                DataPr[1]=DellData[i+1];
                DataPr[2]=DellData[i+2];
                DataPr[3]=DellData[i+3];
                DataPr[4]=DellData[i+4];
                DataPr[5]=DellData[i+5];
                DataPr[6]=DellData[i+6];
                DataPr[7]=DellData[i+7];
                DataPr2=(DataPr).toUInt(&ok,16);
                DataPr.clear();
                DataDec.append(QString().sprintf("%u",DataPr2)).append(" ");
                DataPr.clear();
                i=i+8;
                }
            }
          DellData=DataDec;
          DataDec.clear();
         }
     //SetDec------------------------------

      //Set 8Dec2------------------------
       if((FormatCounter == 9)&&(F8_16_32Param==0))
          {
           i=0;
          while(DellData[i]!='\0')
          {
              if((DellData[i]==' '))
                  {
                  i++;
                  }
              else
                  {
                  DataPr.clear();
                  DataPr[0]=DellData[i];
                  DataPr[1]=DellData[i+1];
                  DataPr2=(DataPr).toUInt(&ok,16);
                  DataPr.clear();
                  DataDec.append(QString().sprintf("%d",(signed char)DataPr2)).append(" ");
                  DataPr.clear();
                   i=i+2;
                  }
              }
            DellData=DataDec;
            DataDec.clear();
           }
       //Set 16Dec2
       if((FormatCounter == 10)&&(F8_16_32Param==1))
          {
           i=0;
          while(DellData[i]!='\0')
          {
              if((DellData[i]==' '))
                  {
                  i++;
                  }
              else
                  {

                  DataPr.clear();
                  DataPr[0]=DellData[i];
                  DataPr[1]=DellData[i+1];
                  DataPr[2]=DellData[i+2];
                  DataPr[3]=DellData[i+3];
                  //qDebug()<<DataPr<<"DataPr";
                  DataPr2=(DataPr).toUInt(&ok,16);
                  //qDebug()<<DataPr2<<QString::number(DataPr2);
                  DataPr.clear();
                  DataDec.append(QString().sprintf("%d",(signed short)DataPr2)).append(" ");
                  DataPr.clear();
                  i=i+4;
                  }
              }
            DellData=DataDec;
            DataDec.clear();
           }
       //Set 32Dec2
       if((FormatCounter == 11)&&(F8_16_32Param==2))
          {
           i=0;
          while(DellData[i]!='\0')
          {
              if((DellData[i]==' '))
                  {
                  i++;
                  }
              else
                  {
                  DataPr.clear();
                  DataPr[0]=DellData[i];
                  DataPr[1]=DellData[i+1];
                  DataPr[2]=DellData[i+2];
                  DataPr[3]=DellData[i+3];
                  DataPr[4]=DellData[i+4];
                  DataPr[5]=DellData[i+5];
                  DataPr[6]=DellData[i+6];
                  DataPr[7]=DellData[i+7];
                  //qDebug()<<DataPr<<"DataPr";
                  DataPr2=(DataPr).toUInt(&ok,16);
                  //qDebug()<<DataPr2<<QString::number(DataPr2);
                  DataPr.clear();
                  DataDec.append(QString().sprintf("%d",(signed int)DataPr2)).append(" ");
                  DataPr.clear();
                  i=i+8;
                  }
              }
            DellData=DataDec;
            DataDec.clear();
           }
       //SetDec2------------------------------

     //Set 8Bin-----------------------------
     if ((FormatCounter == 3)&&(F8_16_32Param == 0))
     {
         j=0;
         while(DellData[j]!='\0')
         {
             if(DellData[j]==' ')
             {
                 j++;
             }
             else
             {
                 DataPr.clear();
                 DataPr[0]=DellData[j];
                 DataPr[1]=DellData[j+1];
                 //qDebug()<<DataPr;
                 DataPr2=(DataPr).toUInt(&ok,16);
                 //qDebug()<<DataPr2;
                 DataPr.clear();
                 itoa(DataPr2,DataPr3,2);
                 AddZero=QString(DataPr3);
                 ZeroSize=AddZero.size();
                 for(int a=0;a<(8-ZeroSize);a++)
                 {
                     AddZero.insert(0,"0");
                 }
                 DataBit.append(AddZero).append(" ");
                 AddZero.clear();
                 j=j+2;
             }
         }
         DellData=DataBit;
         DataBit.clear();
     }
     //Set 16Bin
     if ((FormatCounter == 5)&&(F8_16_32Param == 1))
     {
         j=0;
         while(DellData[j]!='\0')
         {
             if(DellData[j]==' ')
             {
                 j++;
             }
             else
             {
                 DataPr.clear();
                 DataPr[0]=DellData[j];
                 DataPr[1]=DellData[j+1];
                 DataPr[2]=DellData[j+2];
                 DataPr[3]=DellData[j+3];
                 DataPr2=(DataPr).toUInt(&ok,16);
                 DataPr.clear();
                 itoa(DataPr2,DataPr3,2);
                 AddZero=QString(DataPr3);
                 ZeroSize=AddZero.size();
                 for(int a=0;a<(16-ZeroSize);a++)
                 {
                     AddZero.insert(0,"0");
                 }
                 DataBit.append(AddZero).append(" ");
                 AddZero.clear();
                 DataPr.clear();
                 j=j+4;
             }
         }
         DellData=DataBit;
         DataBit.clear();
     }
     //Set 32Bin
     if ((FormatCounter == 7)&&(F8_16_32Param == 2))
     {
         j=0;
         while(DellData[j]!='\0')
         {
             if(DellData[j]==' ')
             {
                 j++;
             }
             else
             {
                 DataPr.clear();
                 DataPr[0]=DellData[j];
                 DataPr[1]=DellData[j+1];
                 DataPr[2]=DellData[j+2];
                 DataPr[3]=DellData[j+3];
                 DataPr[4]=DellData[j+4];
                 DataPr[5]=DellData[j+5];
                 DataPr[6]=DellData[j+6];
                 DataPr[7]=DellData[j+7];
                 DataPr2=(DataPr).toUInt(&ok,16);
                 DataPr.clear();
                 itoa(DataPr2,DataPr3,2);
                 AddZero=QString(DataPr3);
                 ZeroSize=AddZero.size();
                 for(int a=0;a<(32-ZeroSize);a++)
                 {
                     AddZero.insert(0,"0");
                 }
                 DataBit.append(AddZero).append(" ");
                 AddZero.clear();
                 DataPr.clear();
                 j=j+8;
             }
         }
         DellData=DataBit;
         DataBit.clear();
     }


     try
     {
             float *FloatData;
             //Set 8float------------------------
              //Set 16float
              if((FormatCounter == 56)&&(F8_16_32Param==1))
                 {
                  i=0;
                 while(DellData[i]!='\0')
                 {
                     if((DellData[i]==' '))
                         {
                             i++;
                         }
                     else
                         {
                             DataPr.clear();
                             DataPr[0]=DellData[i];
                             DataPr[1]=DellData[i+1];
                             DataPr[2]=DellData[i+2];
                             DataPr[3]=DellData[i+3];
                             DataPr2=(DataPr).toInt(&ok,16);
                             FloatData =(float*)(&DataPr2);
                             DataPr.clear();
                             DataDec.append(QString().sprintf("%g",*FloatData)).append(" ");
                             DataPr.clear();
                             i=i+4;
                         }
                     }
                   DellData=DataDec;
                   DataDec.clear();
                  }
              //Set 32float
              if((FormatCounter == 57)&&(F8_16_32Param==2))
                 {
                  i=0;
                 while(DellData[i]!='\0')
                 {
                     if((DellData[i]==' '))
                         {
                             i++;
                         }
                     else
                         {
                             DataPr.clear();
                             DataPr[0]=DellData[i];
                             DataPr[1]=DellData[i+1];
                             DataPr[2]=DellData[i+2];
                             DataPr[3]=DellData[i+3];
                             DataPr[4]=DellData[i+4];
                             DataPr[5]=DellData[i+5];
                             DataPr[6]=DellData[i+6];
                             DataPr[7]=DellData[i+7];
                             DataPr2=(DataPr).toInt(&ok,16);
                             FloatData =(float*)(&DataPr2);
                             DataPr.clear();
                             DataDec.append(QString().sprintf("%g",*FloatData)).append(" ");
                             DataPr.clear();
                             i=i+8;
                         }
                     }
                   DellData=DataDec;
                   DataDec.clear();
                  }
              //SetFloat------------------------------
     }
     catch(...) {qDebug()<<"Float reading err";};

     MemoryContentData=DellData;
}

void MemoryContentTable::ChangeInfo()
{
    /*
     Промежуточная обработка информаии, мб использована в потомках
    */
}

void MemoryContentTable::FillTable()
{
    ChangeInfo();
    ui->InfoTable->blockSignals(true);
    QString LocalMemoryContentData=QString(MemoryContentData);
    QString CurrentCellInsertInfo;
    int MemoryContentDataCounter=0;
    if(MemoryContentData.size()!=0)
    {
        for(int Row=0;Row<ui->InfoTable->rowCount();Row++)
        {
            for(int Column=0;Column<ui->InfoTable->columnCount();Column++)
            {
                while((LocalMemoryContentData[MemoryContentDataCounter]!=' ')&&(LocalMemoryContentData[MemoryContentDataCounter]!='\0'))
                {
                    CurrentCellInsertInfo.append(LocalMemoryContentData[MemoryContentDataCounter]);
                    MemoryContentDataCounter++;
                }
                ui->InfoTable->item(Row,Column)->setText(CurrentCellInsertInfo);
                if(LocalMemoryContentData[MemoryContentDataCounter]!='\0') MemoryContentDataCounter++;
                CurrentCellInsertInfo.clear();
            }
        }
    }
    else
    {
        for(int Row=0;Row<ui->InfoTable->rowCount();Row++)
        {
            for(int Column=0;Column<ui->InfoTable->columnCount();Column++)
            {
                ui->InfoTable->item(Row,Column)->setText("empty");
            }
        }
    }
    ui->InfoTable->blockSignals(false);
}

void MemoryContentTable::GetAddressParamList()
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
void MemoryContentTable::closeEvent(QCloseEvent *e)
{
    MemoryContentTimer->stop();

}

void MemoryContentTable::wheelEvent(QWheelEvent *event)
{
    int SetAddr;
    int Rez=event->delta()/120;
    bool ok;
    QString Address=ui->AddressLine->text();
    if((Address[0]=='0')&&(Address[1]=='x'))
    {
        SetAddr=AddressLineInfo;
    }
    else
    {
       varlistelm_t *RecombAddress = new varlistelm_t;
       bool update=false;
       dbal_fillfromdb(Address,RecombAddress,update);
       SetAddr=RecombAddress->addr;
    }
    if(Rez<0)
    {
        if(F8_16_32Param == 0)
        {
            SetAddr=SetAddr+WidthElements;
        }
        if(F8_16_32Param == 1)
        {
            SetAddr=SetAddr+WidthElements*2;
         }
        if(F8_16_32Param == 2)
        {
            SetAddr=SetAddr+WidthElements*4;
        }
        ui->AddressLine->setText(QString().sprintf("0x%08X",SetAddr));
    }
    else
    {
        if(F8_16_32Param == 0)
        {
            SetAddr=SetAddr-WidthElements;
        }
        if(F8_16_32Param == 1)
        {
            SetAddr=SetAddr-WidthElements*2;
         }
        if(F8_16_32Param == 2)
        {
            SetAddr=SetAddr-WidthElements*4;
        }
        ui->AddressLine->setText(QString().sprintf("0x%08X",SetAddr));
    }
}

void MemoryContentTable::SaveSelectedCellToBuffer()
{
    QString cbStr;
    QClipboard *cb = QApplication::clipboard();
    QModelIndexList list = ui->InfoTable->selectionModel()->selectedIndexes();
    int i,j, firstRow, lastRow, rowCount;
    if(list.isEmpty()) return;

    firstRow = list.first().row() ;
    lastRow = list.last().row();
    rowCount = lastRow-firstRow+1;

    for(i=0;i<rowCount;++i,cbStr+='\n')
        for(j=i;j<list.count();j+=rowCount,cbStr+='\t')
            cbStr+=ui->InfoTable->model()->data(list[j],Qt::EditRole).toString();
    cb->setText(cbStr);
}
