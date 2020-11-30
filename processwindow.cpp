#include "processwindow.h"
#include "ui_processwindow.h"

const int ProcessWindowTimerInterval=500;

ProcessWindow::ProcessWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProcessWindow)
{
    ProcessWindowSocket=new QTcpSocket(this);
    ui->setupUi(this);
    GetParamToComboBox();
    //ProcessWindowSocket
    ProcessWindowTimer = new QTimer(this);
    ProcessWindowTimer->setInterval(ProcessWindowTimerInterval);
    connect(ProcessWindowTimer,SIGNAL(timeout()),this,SLOT(StartWorking()));
    //Table
    ui->tableWidget->setRowCount(1);
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->horizontalHeader()->hide();
    ui->tableWidget->verticalHeader()->hide();
    ui->StateLabel->setText("State:NotConnected");
    ui->StateLabel->setStyleSheet("background-color: red");
    ui->StateButton->setEnabled(false);
    //блокируем книпки и разблокируем только если есть подключение
    ui->StartButton->setEnabled(false);
    ui->StopButton->setEnabled(false);
    ui->StepButton->setEnabled(false);
    ui->changePCButton->setEnabled(false);
    //Table
    ui->tableWidget->setItem(0,0,new QTableWidgetItem("Address"));
    ui->tableWidget->item(0,0)->setFlags(!Qt::ItemIsEditable);
    ui->tableWidget->setItem(0,1,new QTableWidgetItem("Code"));
    ui->tableWidget->item(0,1)->setFlags(!Qt::ItemIsEditable);
    ui->tableWidget->setItem(0,2,new QTableWidgetItem("CommandAndInstructions"));
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableWidget->item(0,2)->setFlags(!Qt::ItemIsEditable);
    AddRow();
    ui->tableWidget->verticalScrollBar()->hide();
    ui->CurrentAddressLine->setText(QString().sprintf("0x%08X ",0));
    connect(ui->CurrentAddressLine,SIGNAL(textEdited(QString)),this,SLOT(GetAddressParamList()));
    ui->menubar->hide();
    ui->statusbar->hide();
    //contextMenu
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowContextMenu(const QPoint&)));
    //Compliter
    connect(ui->CurrentAddressLine,SIGNAL(textEdited(QString)),this,SLOT(GetAddressParamList()));

    for(int i=0;i<10;i++)//Задаем начальное значение для того ,чтобы не подсвечивались BreakPoint
    {
        BreakPoint[i]=-2;
    }
}

ProcessWindow::~ProcessWindow()
{
    delete ui;
}

void ProcessWindow::StartWorking()
{
    /*--------------------------------------------------
     Программа запускается когда выбран элемент ComboBox
     ---------------------------------------------------*/
    ProcessWindowTimer->stop();
    ReSizeAndAddRow();
    SearchSocket();
    SocketCheckState();
    ProcessWindowTimer->start();
}
void ProcessWindow::SearchSocket()
{
    int QuantitiIterator=0;
    //Ищем подходящий сокет, определяем параметр для проверки переключения канала
    while(!Quantiti[QuantitiIterator].Flag==0)
    {
        if((IP==Quantiti[QuantitiIterator].IpAddr)&&(PORT==Quantiti[QuantitiIterator].Port))
        {
            ProcessWindowSocket=&(Quantiti[QuantitiIterator].IpPortSocket);
            NumberOfChannelControl=Quantiti[QuantitiIterator].NumberOfChannal;
        }
        QuantitiIterator++;
    }
}
void ProcessWindow::SocketCheckState()
{
    //Socket connected?
    switch(ProcessWindowSocket->state())
    {
    case(QAbstractSocket::ConnectedState):
        ui->StateLabel->setText("State:Connected");
        ui->StateLabel->setStyleSheet("background-color: green");
        ui->StartButton->setEnabled(true);
        ui->StopButton->setEnabled(true);
        ui->StepButton->setEnabled(true);
        ui->changePCButton->setEnabled(true);
        ChannelCheck();
        TakePCaddress();
        SetCurrentAddress();
        HaltResumeStateCheck();
        ReadTableParametrs();
        FormAndSendCommand();
        GetInfoFromCell();
        break;
    case(QAbstractSocket::UnconnectedState):
        ClearUiInfo();
        //Connect
        ProcessWindowSocket->connectToHost(QString("%0").arg(IP),PORT);
        ui->StateLabel->setText("State:NotConnected");
        ui->StateLabel->setStyleSheet("background-color: red");
        ui->StartButton->setEnabled(false);
        ui->StopButton->setEnabled(false);
        ui->StepButton->setEnabled(false);
        ui->changePCButton->setEnabled(false);
        break;
    case(QAbstractSocket::ConnectingState):
        ClearUiInfo();
        break;
    default:
        break;
    }
}

void ProcessWindow::SetCurrentAddress()
{
    QLineEdit *CurrentRowLine;
    bool ok;
    bool GoToPc=false;
    int CurrentAddress=0;
    //if radio button is checked
    if(ui->TrackPcRadioButton->isChecked())
    {
        //Ищем PC для отслеживания
        for(int i=1;i<ui->tableWidget->rowCount();i++)
        {
            CurrentRowLine = qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(i,0));
            if(CurrentRowLine->text().toInt(&ok,16)==ui->PcLine->text().toInt(&ok,16))
            {
                GoToPc=true;
            }
        }
        //Если не нашли, то перескакиваем на PC-4
        if(GoToPc==false)
        {
            CurrentAddress=(ui->PcLine->text().toInt(&ok,16))-4;
            ui->CurrentAddressLine->setText(QString().sprintf("0x%08X ",CurrentAddress));
        }
        else
        {
            CurrentAddress=ui->CurrentAddressLine->text().toInt(&ok,16);
        }
    }
    else
    {
        QString CurAddString=ui->CurrentAddressLine->text();
        if((CurAddString[0]=='0')&&(CurAddString[1]=='x'))
        {
            CurrentAddress=ui->CurrentAddressLine->text().toInt(&ok,16);
        }
        else
        {
           varlistelm_t *RecombAddress = new varlistelm_t;
           bool update=false;
           dbal_fillfromdb(CurAddString,RecombAddress,update);
           CurrentAddress=RecombAddress->addr;
        }
       //CurrentAddress=ui->CurrentAddressLine->text().toInt(&ok,16);
    }

    //Заполнение столбцов
    for(int i=1;i<ui->tableWidget->rowCount();i++)
    {
        CurrentRowLine = qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(i,0));
        //CurrentAddress=ui->CurrentAddressLine->text().toInt(&ok,16);
        CurrentAddress=CurrentAddress & 0xfffffffe; // Обнуляем последнеий бит
        CurrentRowLine->setText(QString().sprintf("0x%08X ",(CurrentAddress)+((i-1)*2)));
    }
}

void ProcessWindow::ReadTableParametrs()
{

    QLineEdit *CurrentRowLine;
    int RowInfoStructIterator=0;
    for(int RowIterator=1;RowIterator<ui->tableWidget->rowCount();RowIterator++)
    {
        CurrentRowLine = qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(RowIterator,0));
        RowInfoStruct[RowInfoStructIterator].Address=CurrentRowLine->text();
        RowInfoStructIterator++;
    }
}
void ProcessWindow::FormAndSendCommand()
{
    int LocalAddress;
    bool ok;
    for(int RowIterator=1;RowIterator<ui->tableWidget->rowCount();RowIterator++)
    {
        LocalAddress=RowInfoStruct[RowIterator-1].Address.toInt(&ok,16);
        ProcessWindowSocket->write(QString().sprintf("capture \"arm disassemble 0x%X\"\x1A",LocalAddress));
        ReadFromSocket();

        //Invalid chech
        QString Inv = "Invalid";
        if((RowIterator==1)&&(QString(ProcessWindowData).contains(Inv,Qt::CaseInsensitive))) return;

        FillStructure(RowIterator-1);
        ProcessWindowData.clear();
    }
}

void ProcessWindow::FillStructure(int CurrentRow)
{
    //Заполняем структуры в уоторх содержится информация : адрес,код, команды и инструкции
    int NumOfProcessWindowData=0;
    int AddCodeCommandInstrControl=0;
    int LocalIterator=0;
    while(ProcessWindowData[NumOfProcessWindowData]!='\0')
    {
        switch(AddCodeCommandInstrControl)
        {
        case(0):
            {
                while(ProcessWindowData[NumOfProcessWindowData]!=' ')
                {
                    RowInfoStruct[CurrentRow].Address[LocalIterator]=ProcessWindowData[NumOfProcessWindowData];
                    NumOfProcessWindowData++;
                    LocalIterator++;
                }
                LocalIterator=0;
                AddCodeCommandInstrControl=1;
                break;
            }
        case(1):
            {
                while(ProcessWindowData[NumOfProcessWindowData]==' ') NumOfProcessWindowData++;
                while(ProcessWindowData[NumOfProcessWindowData]!='\x9')
                {
                    RowInfoStruct[CurrentRow].Code[LocalIterator]=ProcessWindowData[NumOfProcessWindowData];
                    NumOfProcessWindowData++;
                    LocalIterator++;
                }
                LocalIterator=0;
                AddCodeCommandInstrControl=2;
                break;
            }
        case(2):
            {
                while(ProcessWindowData[NumOfProcessWindowData]==' ') NumOfProcessWindowData++;
                while(ProcessWindowData[NumOfProcessWindowData]!='\0')
                {
                    RowInfoStruct[CurrentRow].CommandAndInstruction[LocalIterator]=ProcessWindowData[NumOfProcessWindowData];
                    NumOfProcessWindowData++;
                    LocalIterator++;
                }
                break;
            }
        }
        //NumOfProcessWindowData++;
    }
    RowInfoStruct[CurrentRow].Address.replace(' ',"");
    RowInfoStruct[CurrentRow].CommandAndInstruction.replace(" ","");
    RowInfoStruct[CurrentRow].CommandAndInstruction.replace("\x1A","");
    RowInfoStruct[CurrentRow].CommandAndInstruction.replace("\x9"," ");
    //Proverka

}
void ProcessWindow::GetInfoFromCell()
{
    //информацию в ячейку вставляем в соответствии с тем видом который выбирает пользователь
    // ищем и подсвечиваем текущее PC  и точки останова
    MarkBreakPoint();
    QLineEdit *AddressLine;
    for(int RowIterator=1;RowIterator<ui->tableWidget->rowCount();RowIterator++)
    {
        bool ok;
        if(RowInfoStruct[RowIterator-1].Address.toInt(&ok,16)==CurrentPC.toInt(&ok,16))//Подсветить PC
        {
            ui->tableWidget->item(RowIterator,1)->setBackgroundColor(Qt::green);
            ui->tableWidget->item(RowIterator,2)->setBackgroundColor(Qt::green);
        }
        else if(RowInfoStruct[RowIterator-1].Address.toInt(&ok,16)==BreakPoint[0]) //Подсветить BreakPoint 1
        {
            ui->tableWidget->item(RowIterator,1)->setBackgroundColor(Qt::yellow);
            ui->tableWidget->item(RowIterator,2)->setBackgroundColor(Qt::yellow);
        }
        else if(RowInfoStruct[RowIterator-1].Address.toInt(&ok,16)==BreakPoint[1]) //Подсветить BreakPoint 2
        {
            ui->tableWidget->item(RowIterator,1)->setBackgroundColor(Qt::yellow);
            ui->tableWidget->item(RowIterator,2)->setBackgroundColor(Qt::yellow);
        }
        else if(RowInfoStruct[RowIterator-1].Address.toInt(&ok,16)==BreakPoint[2]) //Подсветить BreakPoint 3
        {
            ui->tableWidget->item(RowIterator,1)->setBackgroundColor(Qt::yellow);
            ui->tableWidget->item(RowIterator,2)->setBackgroundColor(Qt::yellow);
        }
        else if(RowInfoStruct[RowIterator-1].Address.toInt(&ok,16)==BreakPoint[3]) //Подсветить BreakPoint 4
        {
            ui->tableWidget->item(RowIterator,1)->setBackgroundColor(Qt::yellow);
            ui->tableWidget->item(RowIterator,2)->setBackgroundColor(Qt::yellow);
        }
        else
        {
            ui->tableWidget->item(RowIterator,1)->setBackgroundColor(Qt::white);
            ui->tableWidget->item(RowIterator,2)->setBackgroundColor(Qt::white);
        }
        AddressLine = qobject_cast<QLineEdit *>(ui->tableWidget->cellWidget(RowIterator,0));
        AddressLine->setText(RowInfoStruct[RowIterator-1].Address);
        ui->tableWidget->item(RowIterator,1)->setText(RowInfoStruct[RowIterator-1].Code);
        ui->tableWidget->item(RowIterator,2)->setText(RowInfoStruct[RowIterator-1].CommandAndInstruction);
    }
    //ClearStructure
    for(int ClearStruct=0;ClearStruct<ProcessWindowMaxRow;ClearStruct++)
    {
        RowInfoStruct[ClearStruct].Address.clear();
        RowInfoStruct[ClearStruct].Code.clear();
        RowInfoStruct[ClearStruct].CommandAndInstruction.clear();
    }
}

void ProcessWindow::ChannelCheck()
{
    bool CheckSearchChannel=false;
    bool StartToReWrite=false;
    bool ok;
    QString CurrentChannel;
    int CurrentChannelIterator=0;
    int ProcessWindowDataIterator=0;
    if(NumberOfChannelControl>1)
    {
        ProcessWindowData.clear();
        ProcessWindowSocket->write(QString().sprintf("capture \"pcilink_setchan %d\"\x1A",CHANNEL));
        ReadFromSocket();
        ProcessWindowData.replace(" ","");
        ProcessWindowData.replace("\x1A","");
        while(ProcessWindowData[ProcessWindowDataIterator]!='\0')
        {
            if(StartToReWrite==true)
            {
                CurrentChannel[CurrentChannelIterator]=ProcessWindowData[ProcessWindowDataIterator];
                CurrentChannelIterator++;
            }
            if(ProcessWindowData[ProcessWindowDataIterator]=='=') StartToReWrite=true;
            ProcessWindowDataIterator++;
        }
        // использую повторно CurrentChannelIterator не как итератор
        CurrentChannelIterator=CurrentChannel.toInt(&ok,10);
        if(CurrentChannelIterator==CHANNEL) CheckSearchChannel=true;
    }else CheckSearchChannel=true;
    ProcessWindowData.clear();
    //if(CheckSearchChannel==false) QMessageBox::warning(this,"Channal warning!!!","Do not connect to channel");
}

void ProcessWindow::TakePCaddress()
{
    ProcessWindowData.clear();
    CurrentPC.clear();
    ProcessWindowSocket->write(QString().sprintf("capture \"reg pc\"\x1A"));
    ReadFromSocket();
    int ProcessWindowDataIterator=0;
    int CurrentPCIterator=0;
    bool StartToReWrite=false;
    while(ProcessWindowData[ProcessWindowDataIterator]!='\0')
    {
        if(StartToReWrite==true)
        {
            CurrentPC[CurrentPCIterator]=ProcessWindowData[ProcessWindowDataIterator];
            CurrentPCIterator++;
        }
        if(ProcessWindowData[ProcessWindowDataIterator]==':') StartToReWrite=true;
        ProcessWindowDataIterator++;
    }
    CurrentPC.replace(' ',"");
    CurrentPC.replace("\x1A","");
    ui->PcLine->setText(CurrentPC);
    ProcessWindowData.clear();
}
void ProcessWindow::ReadFromSocket()
{
    bool exitCounter=false;
    int iter=0;
    QByteArray SocReadInfo;
    while(ProcessWindowData.size()<=20000)
    {
        //CellSocket->waitForReadyRead(100);
        //Ждем 5 сек и отключаемся если socket не отвечает
        if(ProcessWindowSocket->waitForReadyRead(5000)){}
        else
        {
            ProcessWindowData.clear();
            ProcessWindowSocket->disconnectFromHost();
            break;
        }
        //Reading
        SocReadInfo=ProcessWindowSocket->peek(ProcessWindowSocket->bytesAvailable());
        for(iter=0;iter<SocReadInfo.size();iter++)
        {
            if(SocReadInfo[iter]=='\x1A')
            {
                exitCounter=true;
                ProcessWindowData.append(ProcessWindowSocket->read(iter=iter+1));
                break;
            }
        }
        SocReadInfo.clear();
        if(exitCounter==true) break;
    }
}
void ProcessWindow::HaltResumeStateCheck()
{
    if(ProcessWindowSocket->state()==QAbstractSocket::ConnectedState)
    {
        ProcessWindowTimer->stop();
        ProcessWindowData.clear();
        QString TargetName;
        //Получаем имя устройства
        ProcessWindowSocket->write(QString().sprintf("capture \"target current\"\x1A"));
        ReadFromSocket();
        ProcessWindowData.replace(" ","");
        ProcessWindowData.replace("\x1A","");
        TargetName=ProcessWindowData;
        ProcessWindowData.clear();
        //Проверяем его состояние
        ProcessWindowSocket->write(QString("capture \"%0 curstate\"\x1A").arg(TargetName));
        ReadFromSocket();
        ProcessWindowData.replace(" ","");
        ProcessWindowData.replace("\x1A","");
        if(ProcessWindowData=="halted")
        {
            ui->StateButton->setText("halted");
            ui->StateButton->setBackgroundColor(Qt::yellow);
        }
        if(ProcessWindowData=="running")
        {
            ui->StateButton->setText("running");
            ui->StateButton->setBackgroundColor(Qt::cyan);
        }
        ProcessWindowData.clear();
        ProcessWindowTimer->start();
    }
}

void ProcessWindow::GetParamToComboBox()
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
    ui->comboBox->addItem("ConnectionSetting");
    ui->comboBox->addItem("____________________________________");
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
            ui->comboBox->addItem(Name,QVariant(NameCounter));
            NameCounter++;
        }
        i++;
        ui->comboBox->addItem("------------------------------------------------------");
    }
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ComboBoxChangeParam(int)));
}
void ProcessWindow::ComboBoxChangeParam(int index)
{
    //stop timer
   ProcessWindowTimer->stop();;
   IP.clear();
   PORT=0;
   int i=0;
   int C1=0;
   QString Par;
   QString PortChanPr;
   int value=ui->comboBox->itemData(index).toInt();//Определяем количество каналов, чтобы исключить переключение, если подключено 1 устройство
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
           CHANNEL=PortChanPr.toInt();
           C1=0;
           PortChanPr.clear();
       }
       i++;
   }
   //qDebug()<<IP;
   //qDebug()<<QString::number(PORT);
   //qDebug()<<QString::number(CHANNEL);
   //ProcessWindowSocket->disconnectFromHost();
   ClearUiInfo();
   //start timer
   ProcessWindowTimer->start();
}
void ProcessWindow::ClearUiInfo()
{
    ui->PcLine->clear();
}
void ProcessWindow::ReSizeAndAddRow()
{
     //смотрим сколько строк может поместиться в виджет ,ищем разницу и добавляем/удаляем их
     int RowCount=ui->tableWidget->rowCount();
     int CellHeight = ui->tableWidget->cellWidget(1,0)->height()+1;
     int TableWidgetHeight = ui->tableWidget->height();
     int Difference =(TableWidgetHeight-RowCount*CellHeight)/CellHeight;
     if(Difference>=0)
     {
         for(int RowAddCounter=0;RowAddCounter<Difference;RowAddCounter++)
         {
             AddRow();
         }
     }
     else
     {
         for(int RowRemoveCounter=0;RowRemoveCounter>Difference;RowRemoveCounter--)
         {
             if(ui->tableWidget->rowCount()>2)
             {
                 ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);
             }
         }
     }
}
void ProcessWindow::AddRow()
{
    QTableWidgetItem *TableItem;
    QLineEdit* TableLine = new QLineEdit(this);
    bool ok;
    TableLine->setText(QString().sprintf("0x%08X ",(ui->CurrentAddressLine->text().toInt(&ok,16))+((ui->tableWidget->rowCount()-1)*2)));
    TableLine->setEnabled(false);
    connect(TableLine,SIGNAL(textEdited(QString)),this,SLOT(GetAddressParamList()));
    TableLine->setBackgroundColor(Qt::green);
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount()-1,0,TableLine);

    for(int colcount=1;colcount<ui->tableWidget->columnCount();colcount++)
    {
        if(!(ui->tableWidget->item(ui->tableWidget->rowCount()-1,colcount)))
        {
               TableItem = new QTableWidgetItem("value");
               TableItem->setBackgroundColor(Qt::gray);
               ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,colcount,TableItem);
        }
    }
}

void ProcessWindow::closeEvent(QCloseEvent *e)
{
    ProcessWindowTimer->stop();
    delete ui;
}

void ProcessWindow::on_StartButton_clicked()
{
    //Запустить процессор
    SetResume();
}
void ProcessWindow::SetResume()
{
    if(ProcessWindowSocket->state()==QAbstractSocket::ConnectedState)
    {
            ProcessWindowTimer->stop();
            ProcessWindowData.clear();
            ProcessWindowSocket->write(QString().sprintf("capture \"resume\"\x1A"));
            ReadFromSocket();
            ProcessWindowData.clear();
            ProcessWindowTimer->start();
    }
}
void ProcessWindow::SetHalt()
{
    if(ProcessWindowSocket->state()==QAbstractSocket::ConnectedState)
    {
            ProcessWindowTimer->stop();
            ProcessWindowData.clear();
            ProcessWindowSocket->write(QString().sprintf("capture \"halt\"\x1A"));
            ReadFromSocket();
            ProcessWindowData.clear();
            ProcessWindowTimer->start();
    }
}

void ProcessWindow::on_StopButton_clicked()
{
    //Остановить процессор
    SetHalt();
}

void ProcessWindow::GetAddressParamList()
{
    //Выдача листа для автодополнения в соответствии с пользовательским файлом
    QLineEdit* CurrentLine = (QLineEdit*)sender();
    QStringList *CurrentNameList = new QStringList();
    QString text=CurrentLine->text();
    dbal_getsuggestedparamlist(text,CurrentNameList);
    QCompleter* Completer = new QCompleter(*CurrentNameList,this);
    Completer->setCaseSensitivity(Qt::CaseInsensitive);
    CurrentLine->setCompleter(Completer);
}

void ProcessWindow::wheelEvent(QWheelEvent *event)
{
        //Прокручивание мышкой
        int Rez=event->delta()/120;
        bool ok;
        QString Address = ui->CurrentAddressLine->text();
        int SetAddr=0;
        if((Address[0]=='0')&&(Address[1]=='x'))
        {
            SetAddr=(Address).toInt(&ok,16);
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
            ui->CurrentAddressLine->setText(QString().sprintf("0x%08X",(SetAddr)+2));
        }
        else
        {
            if(ui->CurrentAddressLine->text().toInt(&ok,16)>=0)
            {
                ui->CurrentAddressLine->setText(QString().sprintf("0x%08X",(SetAddr)-2));
            }
        }
}
void ProcessWindow::keyPressEvent(QKeyEvent *KeyEvent)
{
    bool ok;
     if(KeyEvent->key() == Qt::Key_PageUp)
     {
         if(ui->CurrentAddressLine->text().toInt(&ok,16)-(ui->tableWidget->rowCount()*2)>=0)
         {
         ui->CurrentAddressLine->setText(QString().sprintf("0x%08X",(ui->CurrentAddressLine->text().toInt(&ok,16))-((ui->tableWidget->rowCount()-1)*2)));
         }
     }
     if(KeyEvent->key()==Qt::Key_PageDown)
     {
         ui->CurrentAddressLine->setText(QString().sprintf("0x%08X",(ui->CurrentAddressLine->text().toInt(&ok,16))+((ui->tableWidget->rowCount()-1)*2)));
     }
}

void ProcessWindow::on_StepButton_clicked()
{
    if(ProcessWindowSocket->state()==QAbstractSocket::ConnectedState)
    {
            ProcessWindowTimer->stop();
            ProcessWindowData.clear();
            ProcessWindowSocket->write(QString().sprintf("capture \"step\"\x1A"));
            ReadFromSocket();
            ProcessWindowData.clear();
            ProcessWindowTimer->start();
    }
}

void ProcessWindow::ShowContextMenu(const QPoint &pos)
{
    int Row = ui->tableWidget->selectionModel()->currentIndex().row();
    int Col = ui->tableWidget->selectionModel()->currentIndex().column();
    if((Col<1)||(Row<1)){}
    else
    {
        QString BreakPointInfoString;
        BreakPoitnInfo();
        BreakPointInfoString=ProcessWindowData;
        ProcessWindowData.clear();
        QPoint globalPos=ui->tableWidget->mapToGlobal(pos);
        ContextMenu = new QMenu(this);
        ContextMenu->addAction("AddBreakPoint",this,SLOT(AddBreakPoint()));
        ContextMenu->addAction("RemoveBreakPoint",this,SLOT(RemoveBreakPoint()));
        ContextMenu->addAction("RemoveAllBreakPoint",this,SLOT(RemoveAllBreakPoint()));;
        ContextMenu->addAction("__BP__::"+BreakPointInfoString);
        ContextMenu->addAction("MovePC",this,SLOT(MovePC()));
        ContextMenu->exec(globalPos);
    }
}
void ProcessWindow::AddBreakPoint()
{
    //Добавить точку останова
    bool ok;
    int Row = ui->tableWidget->selectionModel()->currentIndex().row();
    QLineEdit *AddLine=qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(Row,0));
    int Address=AddLine->text().toInt(&ok,16);
    if(ProcessWindowSocket->state()==QAbstractSocket::ConnectedState)
    {
        ProcessWindowTimer->stop();
        ProcessWindowData.clear();
        ProcessWindowSocket->write(QString().sprintf("capture \"bp 0x%08X 2 hw\"\x1A",Address));
        ReadFromSocket();
        ProcessWindowData.clear();
        ProcessWindowTimer->start();
    }
}
void ProcessWindow::BreakPoitnInfo()
{
    // Выводит информацию о всех точках останова в соответсвующие структуры для дальнейшего использования
     if(ui->comboBox->currentIndex()!=0)
     {
        if(ProcessWindowSocket->state()==QAbstractSocket::ConnectedState)
        {
            int counter=0;
            int counter2=0;
            int control=0;
            ProcessWindowSocket->write(QString().sprintf("capture \"bp\"\x1A"));
            ReadFromSocket();
            QByteArray Address = ProcessWindowData;
            ProcessWindowData.clear();
            while(Address[counter]!='\0')
            {
                if(control==1){ProcessWindowData[counter2]=Address[counter];counter2++;}
                if(Address[counter]==':') control=1;
                if(Address[counter]==',') control=0;
                counter++;
            }
        }
     }
}
void ProcessWindow::RemoveBreakPoint()
{
    //Снимаем определенную точку останова
    bool ok;
    int Row = ui->tableWidget->selectionModel()->currentIndex().row();
    QLineEdit *AddLine=qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(Row,0));
    int Address=AddLine->text().toInt(&ok,16);
    if(ProcessWindowSocket->state()==QAbstractSocket::ConnectedState)
    {
        ProcessWindowTimer->stop();
        ProcessWindowData.clear();
        ProcessWindowSocket->write(QString().sprintf("capture \"rbp 0x%08X\"\x1A",Address));
        ReadFromSocket();
        ProcessWindowData.clear();
        for(int i=0;i<2;i++)
        {
            if(Address==BreakPoint[i])
            {
                BreakPoint[i]=-2;
            }
        }
        ProcessWindowTimer->start();
    }
}

void ProcessWindow::RemoveAllBreakPoint()
{
    // проходимся по структурам с информацией о точках останова и снимаем их все
    MarkBreakPoint();
     for(int i=0; i<10;i++)
    {
            ProcessWindowSocket->write(QString().sprintf("capture \"rbp 0x%08X\"\x1A",BreakPoint[i]));
            ReadFromSocket();
            ProcessWindowData.clear();
            BreakPoint[i]=-2;//Убираем подсветку
    }
}

void ProcessWindow::MarkBreakPoint()
{
    BreakPoitnInfo();
    QString BrPointAddr;
    QString Data=ProcessWindowData;
    ProcessWindowData.clear();
    int BrPointAddeInt=0;
    int counter=0;
    int counter2=0;
    int control=1;
    int BreakPointCounter=0;
    bool ok;
    while(Data[counter]!='\0')
    {
        if(Data[counter]==',') {control=0;}
        if(control==1)
        {
          BrPointAddr[counter2]=Data[counter];
          counter2++;
        }
        if(control==0)
        {
            BrPointAddeInt=BrPointAddr.toInt(&ok,16);
            BreakPoint[BreakPointCounter]=BrPointAddeInt;
            BrPointAddr.clear();
            counter2=0;
            control=1;
            BreakPointCounter++;
        }
        counter++;
    }
}

void ProcessWindow::on_changePCButton_clicked()
{
    if(ProcessWindowSocket->state()==QAbstractSocket::ConnectedState)
    {

        ProcessWindowTimer->stop();
        bool ok;
        int Add = ui->changePcline->text().toInt(&ok,16);
        ProcessWindowSocket->write(QString().sprintf("capture \"reg pc 0x%X\"\x1A",Add));
        ReadFromSocket();
        ProcessWindowData.clear();
        ProcessWindowTimer->start();
    }
}

void ProcessWindow::MovePC()
{
    //ПОставить PC
    if(ProcessWindowSocket->state()==QAbstractSocket::ConnectedState)
    {
        ProcessWindowTimer->stop();
        bool ok;
        int Row = ui->tableWidget->selectionModel()->currentIndex().row();
        QLineEdit *AddLine=qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(Row,0));
        int Address=AddLine->text().toInt(&ok,16);
        ProcessWindowSocket->write(QString().sprintf("capture \"reg pc 0x%X\"\x1A",Address));
        ReadFromSocket();
        ProcessWindowData.clear();
        ProcessWindowTimer->start();
    }
}
void ProcessWindow::MovePC(QString MoveAddress)
{
    if(ProcessWindowSocket->state()==QAbstractSocket::ConnectedState)
    {
        ProcessWindowTimer->stop();
        bool ok;
        int MoveAddress_Int = MoveAddress.toInt(&ok,16) & 0xfffffffe;
        ProcessWindowSocket->write(QString().sprintf("capture \"reg pc 0x%X\"\x1A",MoveAddress_Int));
        ReadFromSocket();
        ProcessWindowData.clear();
        ProcessWindowTimer->start();
    }
}
