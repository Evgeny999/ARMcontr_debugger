#include "registerswindow.h"
#include "ui_registerswindow.h"

int RegWindowTimerInterval=500;


RegistersWindow::RegistersWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RegistersWindow)
{
    ui->setupUi(this);
    RegWindSocket = new QTcpSocket(this);
    RegWindowTimer = new QTimer(this);
    HaltRunState = false;
    RegTableItem = new QTableWidgetItem("");
    RegWindowTimer->setInterval(RegWindowTimerInterval);
    /*
     Окно регистра начинает свою работу тогда, когда выбран элемент ComboBox
    */
    connect(RegWindowTimer,SIGNAL(timeout()),this,SLOT(StartWorking()));
    ui->menubar->hide();
    ui->statusbar->hide();
    ui->registersTable->verticalHeader()->hide();
    ui->registersTable->horizontalHeader()->hide();
    ui->registersTable->setRowCount(2);
    ui->registersTable->setColumnCount(2);
    ui->registersTable->setItem(0,0,new QTableWidgetItem("Reg"));
    ui->registersTable->item(0,0)->setFlags(!Qt::ItemIsEditable);
    ui->registersTable->setItem(0,1,new QTableWidgetItem("Info"));
    ui->registersTable->item(0,1)->setFlags(!Qt::ItemIsEditable);
    ui->registersTable->setRowCount(101);
    ui->registersTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    for(int i=1;i<101;i++)
    {
       ui->registersTable->setItem(i,0,new QTableWidgetItem(""));
       ui->registersTable->setItem(i,1,new QTableWidgetItem(""));
    }
    //Заполнение комбобокса
    GetParamToComboBox();
}

RegistersWindow::~RegistersWindow()
{
    delete ui;
}
void RegistersWindow::StartWorking()
{
    RegWindowTimer->stop();
    SearchSocket();
    SocketCheckState();
    RegWindowTimer->start();
}

void RegistersWindow::SearchSocket()
{
    int QuantitiIterator=0;
    //Ищем подходящий сокет, определяем параметр для проверки переключения канала
    while(!Quantiti[QuantitiIterator].Flag==0)
    {
        if((IP==Quantiti[QuantitiIterator].IpAddr)&&(PORT==Quantiti[QuantitiIterator].Port))
        {
            RegWindSocket=&(Quantiti[QuantitiIterator].IpPortSocket);
            NumberOfChannelControl=Quantiti[QuantitiIterator].NumberOfChannal;
        }
        QuantitiIterator++;
    }
}

void RegistersWindow::ChannelCheck()
{
    bool CheckSearchChannel=false;
    bool StartToReWrite=false;
    bool ok;
    QString CurrentChannel;
    int CurrentChannelIterator=0;
    int RegWindDataIterator=0;
    if(NumberOfChannelControl>1)
    {
        RegWindData.clear();
        RegWindSocket->write(QString().sprintf("capture \"pcilink_setchan %d\"\x1A",CHANNEL));
        ReadFromSocket();
        RegWindData.replace(" ","");
        RegWindData.replace("\x1A","");
        while(RegWindData[RegWindDataIterator]!='\0')
        {
            if(StartToReWrite==true)
            {
                CurrentChannel[CurrentChannelIterator]=RegWindData[RegWindDataIterator];
                CurrentChannelIterator++;
            }
            if(RegWindData[RegWindDataIterator]=='=') StartToReWrite=true;
            RegWindDataIterator++;
        }
        // использую повторно CurrentChannelIterator не как итератор
        CurrentChannelIterator=CurrentChannel.toInt(&ok,10);
        if(CurrentChannelIterator==CHANNEL) CheckSearchChannel=true;
    }else CheckSearchChannel=true;
    RegWindData.clear();
    //if(CheckSearchChannel==false) QMessageBox::warning(this,"Channal warning!!!","Do not connect to channel");
}

void RegistersWindow::ReadFromSocket()
{
    bool exitCounter=false;
    int iter=0;
    QByteArray SocReadInfo;
    while(RegWindData.size()<=20000)
    {
        //CellSocket->waitForReadyRead(100);
        //Ждем 5 сек и отключаемся если socket не отвечает
        if(RegWindSocket->waitForReadyRead(5000)){}
        else
        {
            RegWindData.clear();
            RegWindSocket->disconnectFromHost();
            break;
        }
        //Reading
        SocReadInfo=RegWindSocket->peek(RegWindSocket->bytesAvailable());
        for(iter=0;iter<SocReadInfo.size();iter++)
        {
            if(SocReadInfo[iter]=='\x1A')
            {
                exitCounter=true;
                RegWindData.append(RegWindSocket->read(iter=iter+1));
                break;
            }
        }
        SocReadInfo.clear();
        if(exitCounter==true) break;
    }
}

void RegistersWindow::SocketCheckState()
{
    switch(RegWindSocket->state())
    {
        case (QAbstractSocket::ConnectedState):
        {
            ChannelCheck();
            //CheckHaltResumStateb and return HaltRunState
            CheckHaltResumState();
            //Если есть подключение и процессор остановлен, то считываем информацию о регистрах
            if(HaltRunState==true)
            {
                SendCommand();
                ReadFromSocket();
                RegWindowFilter();
                InsertInfoIntoTable();
            }
            else
            {
                for(int i=1;i<101;i++)
                {
                    ui->registersTable->item(i,1)->setText("Not halted");
                    ui->registersTable->item(i,1)->setBackgroundColor(Qt::yellow);
                }
            }
            break;
        }
        case(QAbstractSocket::UnconnectedState):
        {
            for(int i=1;i<101;i++)
            {
                ui->registersTable->item(i,1)->setText("Disconnect");
                ui->registersTable->item(i,1)->setBackgroundColor(Qt::red);
            }
            RegWindSocket->connectToHost(QString("%0").arg(IP),PORT);
            break;
        }

        case(QAbstractSocket::ConnectingState):
        {
            for(int i=1;i<101;i++)
            {
                ui->registersTable->item(i,1)->setText("Connecting");
                ui->registersTable->item(i,1)->setBackgroundColor(Qt::red);
            }
             break;
        }
    default:
        break;
    }
}

void RegistersWindow::SendCommand()
{
    RegWindSocket->write(QString().sprintf("capture \"reg\"\x1A"));
}

void RegistersWindow::CheckHaltResumState()
{
    if(RegWindSocket->state()==QAbstractSocket::ConnectedState)
    {
        RegWindowTimer->stop();
        RegWindData.clear();
        QString TargetName;
        //Получаем имя устройства
        RegWindSocket->write(QString().sprintf("capture \"target current\"\x1A"));
        ReadFromSocket();
        RegWindData.replace(" ","");
        RegWindData.replace("\x1A","");
        TargetName=RegWindData;
        RegWindData.clear();
        //Проверяем его состояние
        RegWindSocket->write(QString("capture \"%0 curstate\"\x1A").arg(TargetName));
        ReadFromSocket();
        RegWindData.replace(" ","");
        RegWindData.replace("\x1A","");
        if(RegWindData=="halted")
        {
            HaltRunState = true;
        }
        else if(RegWindData=="running")
        {
            HaltRunState = false;
        }
        else HaltRunState = false;
        RegWindData.clear();
        RegWindowTimer->start();
    }
    else
    {
        HaltRunState = false;
    }
}

void RegistersWindow::RegWindowFilter()
{
    //ClearStruct
    for(int i=0; i<100;i++)
    {
        RegInfoStruct[i].Registr.clear();
        RegInfoStruct[i].RegName.clear();
        RegInfoStruct[i].RegInfo.clear();
        RegInfoStruct[i].Size=0;
    }
    //Write Filter
    //RegInfoStruct[100]
    int RegWindDataCount=0;
    int RegStructCount=0;
    int InfoStructCount=0;
    int StructNumber=0;
    int NameCounter=0;
    QString RegSize;
    int regSizeCount=0;
    //bool StartToRead = false;
    while(RegWindData[RegWindDataCount]!='\0')
    {
        if(RegWindData[RegWindDataCount]=='(') // после того, как появилась первая открывающая скобка считываем информацию о имени регистра
        {
           while(RegWindData[RegWindDataCount]!='\x0A')
            {
               if(RegWindData[RegWindDataCount]==':') //После того, как нашли двоеточие считываем информацию о регистре
               {
                   RegWindDataCount++;
                   while(RegWindData[RegWindDataCount]!='\x0A')
                   {
                       RegInfoStruct[StructNumber].RegInfo[InfoStructCount]=RegWindData[RegWindDataCount];
                       InfoStructCount++;
                       RegWindDataCount++;
                   }
                   RegWindDataCount--;
                   break;
               }
               RegInfoStruct[StructNumber].Registr[RegStructCount]=RegWindData[RegWindDataCount];
               RegWindDataCount++;
               RegStructCount++;
            }
           StructNumber++;
           RegStructCount=0;
           InfoStructCount=0;
        }
        RegWindDataCount++;
    }

    for(int i=0;i<100;i++)
    {
        NameCounter=0;
        RegSize.clear();
        regSizeCount=0;
        bool Read = true;
        InfoStructCount=0;
        while(RegInfoStruct[i].Registr[InfoStructCount]!='\0')
        {
            if(RegInfoStruct[i].Registr[InfoStructCount]==')')// после первой щакрывающей скобки переписываем имя регистра, отдельно от другой информации
            {
                if(Read==true)
                {
                    InfoStructCount++;
                    while(RegInfoStruct[i].Registr[InfoStructCount]!='(')
                    {
                        RegInfoStruct[i].RegName[NameCounter]=RegInfoStruct[i].Registr[InfoStructCount];
                        InfoStructCount++;
                        NameCounter++;
                    }
                    Read=false; // если Read=false;  то не реагируем на остальные закрывающие скобки
                }
            }
            if(RegInfoStruct[i].Registr[InfoStructCount]=='/') // после слеша считываем информацию о размерности регистра
            {
                InfoStructCount++;
                while(RegInfoStruct[i].Registr[InfoStructCount]!=')')
                {
                    RegSize[regSizeCount]=RegInfoStruct[i].Registr[InfoStructCount];
                    InfoStructCount++;
                    regSizeCount++;
                }
                RegInfoStruct[i].Size=RegSize.toInt();
            }
            InfoStructCount++;
        }
    }

    //Вся информация о регистрах записывается в специальную структуру RegInfoStruct

    /*//proverka
    for(int i=0; i<100;i++)
    {
        qDebug()<<i<<"REG"<<RegInfoStruct[i].Registr;
        qDebug()<<i<<"Name"<<RegInfoStruct[i].RegName;
        qDebug()<<i<<"Info"<<RegInfoStruct[i].RegInfo;
        qDebug()<<i<<"Size"<<RegInfoStruct[i].Size;

    }*/

    RegWindData.clear();
}

void RegistersWindow::GetParamToComboBox()
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
void RegistersWindow::ComboBoxChangeParam(int index)
{
    RegWindowTimer->stop();
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
    RegWindowTimer->start();
}

void RegistersWindow::InsertInfoIntoTable()
{
    bool old_blocked = ui->registersTable->blockSignals(true); //block item change siganls
    for(int i=1;i<101;i++)
    {
        ui->registersTable->item(i,0)->setText(RegInfoStruct[i-1].RegName);
        ui->registersTable->item(i,1)->setText(RegInfoStruct[i-1].RegInfo);
        ui->registersTable->item(i,1)->setBackgroundColor(Qt::white);
    }
    ui->registersTable->blockSignals(old_blocked);
}

void RegistersWindow::closeEvent(QCloseEvent *e)
{
   RegWindowTimer->stop();
   delete ui;
}

void RegistersWindow::on_registersTable_itemChanged(QTableWidgetItem* item)
{
    if(RegWindSocket->state()==QAbstractSocket::ConnectedState)
    {
        RegWindowTimer->stop();
        bool ok;
        bool old_blocked = ui->registersTable->blockSignals(true); // block item change signal
        int Row = item->row();
        QString RegName = ui->registersTable->item(Row,0)->text();
        int Addr = item->text().toInt(&ok,16);
        RegWindSocket->write(QString("capture \"reg %0 %1\"\x1A").arg(RegName).arg(QString().sprintf("0x%X",Addr)));
        ReadFromSocket();
        RegWindData.clear();
        ui->registersTable->blockSignals(old_blocked); //Снимаем блок сигнала
        RegWindowTimer->start();
    }
}
