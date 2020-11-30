#include "arraysview.h"
#include "ui_arraysview.h"
const float MemoryWindow_FONT_FORMAT =1;
const int ArrViewWindowTimerInterval = 500;

ArraysView::ArraysView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArraysView)
{
    ui->setupUi(this);
    ArrViewTimer = new QTimer(this);
    ArrViewSocket= new QTcpSocket();
    GetParamToComboBox();
    ui->RazrComboBox->addItem("8");
    ui->RazrComboBox->addItem("16");
    ui->RazrComboBox->addItem("32");
    ui->VIdComboBox->addItem("hex");//3hex
    ui->VIdComboBox->addItem("dec");//2dec Not Znak
    ui->VIdComboBox->addItem("decSign");//Znak
    ui->VIdComboBox->addItem("bin");//1bin

    ui->TextEditInfo->setEnabled(false);
    ui->SizeLine->setFixedWidth(ui->TextEditInfo->fontMetrics().width('0')*7);
    ui->IntervalLine->setFixedWidth(ui->TextEditInfo->fontMetrics().width('0')*7);
    ui->ScmechenieLine->setFixedWidth(ui->TextEditInfo->fontMetrics().width('0')*7);
    ui->AddrLine->setMinimumWidth(ui->TextEditInfo->fontMetrics().width('0')*11);
    CurrentAddress=10;
    VARSize=32;
    Smeschenie=0;
    ui->AddrLine->setText(QString().sprintf("0x%08X",CurrentAddress));
    ui->SizeLine->setText(QString::number(VARSize));
    ui->ScmechenieLine->setText(QString::number(Smeschenie));
    ArrViewTimer->setInterval(ArrViewWindowTimerInterval);
    connect(ArrViewTimer,SIGNAL(timeout()),this,SLOT(StartWorking()));
    connect(ui->AddrLine,SIGNAL(textEdited(QString)),this,SLOT(GetAddressParamList()));
    ui->IntervalLine->setText("0-32");

    ui->TextEditInfo->setEnabled(true);
    ui->TextEditInfo->setReadOnly(true);
    RewriteAction = new ReWriteContMenuWidget();
    RewriteAction->SaveToBufferText=ui->TextEditInfo;
    ui->TextEditInfo->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->TextEditInfo,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(ShowContextMenu(const QPoint&)));
    ContextMenu = new QMenu(this);
    ContextMenu->addAction(RewriteAction);
    connect(RewriteAction->ChangeButton,SIGNAL(clicked()),this,SLOT(ContextMenuButtonClicked()));
}

ArraysView::~ArraysView()
{
    delete ui;
}

void ArraysView::StartWorking()
{
    GetFormat();
    GetSize();
    SearchSocket();
    SocketCheckState();
}

void ArraysView::SearchSocket()
{
    int QuantitiIterator=0;
    while(!Quantiti[QuantitiIterator].Flag==0)
    {
        if((IPADD==Quantiti[QuantitiIterator].IpAddr)&&(PORT==Quantiti[QuantitiIterator].Port))
        {
            ArrViewSocket=&(Quantiti[QuantitiIterator].IpPortSocket);
            NumberOfChannelControl=Quantiti[QuantitiIterator].NumberOfChannal;
        }
        QuantitiIterator++;
    }
}

void ArraysView::SocketCheckState()
{
    switch(ArrViewSocket->state())
    {
        case (QAbstractSocket::ConnectedState):
        {
            ChannelCheck();
            SendCommand();
            ReadFromSocket();
            Filter();
            AddLineUnderInfo();
            ui->TextEditInfo->setText(ArrViewData);
            ArrViewData.clear();
            break;
        }
        case(QAbstractSocket::UnconnectedState):
        {
            ArrViewSocket->connectToHost(QString("%0").arg(IPADD),PORT);
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

void ArraysView::ReadFromSocket()
{
    bool exitCounter=false;
    int iter=0;
    QByteArray SocReadInfo;
    while(ArrViewData.size()<=2000000)
    {
        //CellSocket->waitForReadyRead(100);
        //Ждем 5 сек и отключаемся если socket не отвечает
        if(ArrViewSocket->waitForReadyRead(5000)){}
        else
        {
            ArrViewData.clear();
            ArrViewSocket->disconnectFromHost();
            break;
        }
        //Reading
        SocReadInfo=ArrViewSocket->peek(ArrViewSocket->bytesAvailable());
        for(iter=0;iter<SocReadInfo.size();iter++)
        {
            if(SocReadInfo[iter]=='\x1A')
            {
                exitCounter=true;
                ArrViewData.append(ArrViewSocket->read(iter=iter+1));
                break;
            }
        }
        SocReadInfo.clear();
        if(exitCounter==true) break;
    }
}

void ArraysView::ChannelCheck()
{
        bool CheckSearchChannel=false;
        bool StartToReWrite=false;
        bool ok;
        QString CurrentChannel;
        int CurrentChannelIterator=0;
        int ProcessWindowDataIterator=0;
        if(NumberOfChannelControl>1)
        {
            ArrViewData.clear();
            ArrViewSocket->write(QString().sprintf("capture \"pcilink_setchan %d\"\x1A",CHANNEL));
            ReadFromSocket();
            ArrViewData.replace(" ","");
            ArrViewData.replace("\x1A","");
            while(ArrViewData[ProcessWindowDataIterator]!='\0')
            {
                if(StartToReWrite==true)
                {
                    CurrentChannel[CurrentChannelIterator]=ArrViewData[ProcessWindowDataIterator];
                    CurrentChannelIterator++;
                }
                if(ArrViewData[ProcessWindowDataIterator]=='=') StartToReWrite=true;
                ProcessWindowDataIterator++;
            }
            // использую повторно CurrentChannelIterator не как итератор
            CurrentChannelIterator=CurrentChannel.toInt(&ok,10);
            if(CurrentChannelIterator==CHANNEL) CheckSearchChannel=true;
        }else CheckSearchChannel=true;
        ArrViewData.clear();
}

void ArraysView::SendCommand()
{
    if(ArrViewSocket->state()==QAbstractSocket::ConnectedState)
    {
        int SetAddress;
        int number;
        if (F8_16_32Param == 0)
            {
              SetAddress = CurrentAddress + Smeschenie+LeftInterval;
              number=VARSize-Smeschenie-(VARSize-RightInterval)-LeftInterval;
              ArrViewSocket->write(QString().sprintf("capture \"mdb 0x%X %d\"\x1A",SetAddress,number));
            }
        if (F8_16_32Param == 1)
            {
              SetAddress = CurrentAddress + Smeschenie*2+LeftInterval*2;
              number=VARSize-Smeschenie-(VARSize-RightInterval)-LeftInterval;
              ArrViewSocket->write(QString().sprintf("capture \"mdh 0x%X %d\"\x1A",SetAddress,number));
            }
        if (F8_16_32Param == 2)
            {
              SetAddress = CurrentAddress + Smeschenie*4+LeftInterval*4;
              number=VARSize-Smeschenie-(VARSize-RightInterval)-LeftInterval;
              ArrViewSocket->write(QString().sprintf("capture \"mdw 0x%X %d\"\x1A",SetAddress,number));
            }
    }
}

void ArraysView::GetFormat()
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
     VARSize=ui->SizeLine->text().toInt();
     Smeschenie=ui->ScmechenieLine->text().toInt();
     F8_16_32Param  = ui->RazrComboBox->currentIndex();
     HEXDECBINParam = ui->VIdComboBox->currentIndex();


     QString IntervalInfo=ui->IntervalLine->text();
     QString LeftIntervalInfo;
     QString RightIntervalInfo;
     bool ReadRightInterval=false;
     int i=0;
     int j=0;
     //bool ok=0;
     while(IntervalInfo[i]!='\0')
     {
        if((IntervalInfo[i]==':')||(IntervalInfo[i]=='-'))
         {
            ReadRightInterval=true;
            j=0;
            if(IntervalInfo[i+1]!='\0')
                i++;
         }
        if(ReadRightInterval==false)
            LeftIntervalInfo[j]=IntervalInfo[i];
        else
            RightIntervalInfo[j]=IntervalInfo[i];
        i++;
        j++;
     }
     LeftInterval=LeftIntervalInfo.toInt();
     RightInterval=RightIntervalInfo.toInt();
     if(RightInterval>VARSize-1)
         RightInterval = VARSize-1; //-1 because, for example Size=32 but Interval=0..31


     bool ok;
     QString Address;
     Address=ui->AddrLine->text();
     if((Address[0]=='0')&&(Address[1]=='x'))
     {
         CurrentAddress=ui->AddrLine->text().toInt(&ok,16);
         VARSize=ui->SizeLine->text().toInt();
     }
     else
     {
        bool update=false;
        varlistelm_t *RecombAddress = new varlistelm_t;
        dbal_fillfromdb(Address,RecombAddress,update);
        CurrentAddress=RecombAddress->addr;
     }

     if(F8_16_32Param == 0) //8
     {
         if(HEXDECBINParam == 0){FormatCounter = 20; Kformat=3;} //hex
         if(HEXDECBINParam == 1){FormatCounter = 4; Kformat=4;} //dec
         if(HEXDECBINParam == 2){FormatCounter = 9; Kformat = 5;} //decsign
         if(HEXDECBINParam == 3){FormatCounter = 3; Kformat=9;} //bin
     }
     if(F8_16_32Param == 1) //16
     {
         if(HEXDECBINParam == 0){FormatCounter = 21; Kformat = 5;} //hex
         if(HEXDECBINParam == 1){FormatCounter = 6; Kformat = 6;} //dec
         if(HEXDECBINParam == 2){FormatCounter = 10; Kformat = 7;} //decsign
         if(HEXDECBINParam == 3){FormatCounter = 5; Kformat = 17;} //bin
     }
     if(F8_16_32Param == 2) //32
     {
         if(HEXDECBINParam == 0){FormatCounter = 22; Kformat = 9;} //hex
         if(HEXDECBINParam == 1){FormatCounter = 8; Kformat = 11;} //dec
         if(HEXDECBINParam == 2){FormatCounter = 11; Kformat = 12;} //decsign
         if(HEXDECBINParam == 3){FormatCounter = 7; Kformat = 33;} //bin
     }
}
void ArraysView::GetSize()
{
    //Определяем размеры соот ветствующие разным форматам вывода информации
    Width=ui->TextEditInfo->width();
    Height=ui->TextEditInfo->height();
    int ElemHeight = ui->TextEditInfo->fontMetrics().height();
    int ElemWidth = ui->TextEditInfo->fontMetrics().width('0');
    //Wi=((Width-30-96)/(8*Kformat))*MemoryWindow_FONT_FORMAT;
     float Wi=((Width-30-12*ElemWidth)/(ElemWidth*Kformat))*MemoryWindow_FONT_FORMAT;//New
     WidthElement=floor(Wi);//Width - otstup - add/ Pwight*NumOfElement
     //He=(Height)/16;
     float He=(Height)/ElemHeight; //NEw
     HeightElement=floor(He);
     //Param out
     //qDebug()<<number;
     InfoSize=HeightElement*(12+WidthElement*Kformat);
}

void ArraysView::Filter()
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

    QString LocalData=QString(ArrViewData);
    QString DellData;
    ArrViewData.clear();

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
                DataDec.append(QString().sprintf("%-3u",DataPr2)).append(" ");
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
                DataDec.append(QString().sprintf("%-5u",DataPr2)).append(" ");
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
                DataDec.append(QString().sprintf("%-10u",DataPr2)).append(" ");
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
                  DataDec.append(QString().sprintf("%-4d",(signed char)DataPr2)).append(" ");
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
                  DataDec.append(QString().sprintf("%-6d",(signed short)DataPr2)).append(" ");
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
                  DataDec.append(QString().sprintf("%-11d",(signed int)DataPr2)).append(" ");
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

     //11111111111111111111111111111111
     //SetAddress = CurrentAddress + Smeschenie+LeftInterval;
     int LocalAddress=0;
     if(F8_16_32Param == 0)
     {
          LocalAddress=CurrentAddress+Smeschenie+LeftInterval;
     }
     if(F8_16_32Param == 1)
     {
         LocalAddress=CurrentAddress+Smeschenie*2+LeftInterval*2;
      }
     if(F8_16_32Param == 2)
     {
         LocalAddress=CurrentAddress+Smeschenie*4+LeftInterval*4;
     }
     int LocCoun=0;
     DellData.insert(0,QString().sprintf("0x%08X: ",LocalAddress));
     if(F8_16_32Param == 0)
     {
        LocalAddress=LocalAddress+WidthElement;
     }
     if(F8_16_32Param == 1)
     {
        LocalAddress=LocalAddress+WidthElement*2;
      }
     if(F8_16_32Param == 2)
     {
         LocalAddress=LocalAddress+WidthElement*4;
      }
     for(int i=12;i<DellData.size();i++)
     {

         if(DellData[i]==' ')
         {
                 while(DellData[i+1]==' ')
                 {
                     i++;
                 }

             LocCoun++;
         }
         if(LocCoun>=WidthElement)
         {
             DellData[i]='\n';
             DellData.insert(i+1,QString().sprintf("0x%08X: ",LocalAddress));
             i=i+12;
             LocCoun=0;
             if(F8_16_32Param == 0)
             {
                LocalAddress=LocalAddress+WidthElement;
             }
             if(F8_16_32Param == 1)
             {
                 LocalAddress=LocalAddress+WidthElement*2;
              }
             if(F8_16_32Param == 2)
             {
                 LocalAddress=LocalAddress+WidthElement*4;
              }
         }

     }
     ArrViewData=DellData;
}

void ArraysView::closeEvent(QCloseEvent *)
{
    ArrViewTimer->stop();
}

void ArraysView::GetAddressParamList()
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

void ArraysView::GetParamToComboBox()
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
        ui->SettingComboBox->addItem("ConnectionSetting");
        ui->SettingComboBox->addItem("____________________________________");
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
                ui->SettingComboBox->addItem(Name,QVariant(NameCounter));
                NameCounter++;
            }
            i++;
            ui->SettingComboBox->addItem("------------------------------------------------------");
        }
        connect(ui->SettingComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ComboBoxChangeParam(int)));
}

void ArraysView::ComboBoxChangeParam(int index)
{
    ArrViewTimer->stop();
    IPADD.clear();
    PORT=0;
    int i=0;
    int C1=0;
    QString Par;
    QString PortChanPr;
    int value=ui->SettingComboBox->itemData(index).toInt();//Определяем количество каналов, чтобы исключить переключение, если подключено 1 устройство
    //qDebug()<<NameMass[value];
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
    ArrViewTimer->start();
}

void ArraysView::AddLineUnderInfo()
{
    int F8_16_32 = F8_16_32Param;
    int FHexDecBin = HEXDECBINParam;
    QString UnderInfoText;
    UnderInfoText.append("Address...: ");
    int WordCounter=1;
    for(int i=0;i<WidthElement;i++)
    {
        if(F8_16_32==0)
        {
            WordCounter=1;
            if(FHexDecBin==0) UnderInfoText.append(QString().sprintf("%-3X",i*WordCounter));
            if(FHexDecBin==1) UnderInfoText.append(QString().sprintf("%-4X",i*WordCounter));
            if(FHexDecBin==2) UnderInfoText.append(QString().sprintf("%-5X",i*WordCounter));
            if(FHexDecBin==3) UnderInfoText.append(QString().sprintf("%-9X",i*WordCounter));
        }
        if(F8_16_32==1)
        {
            WordCounter=2;
            if(FHexDecBin==0) UnderInfoText.append(QString().sprintf("%-5X",i*WordCounter));
            if(FHexDecBin==1) UnderInfoText.append(QString().sprintf("%-6X",i*WordCounter));
            if(FHexDecBin==2) UnderInfoText.append(QString().sprintf("%-7X",i*WordCounter));
            if(FHexDecBin==3) UnderInfoText.append(QString().sprintf("%-17X",i*WordCounter));
        }
        if(F8_16_32==2)
        {
            WordCounter=4;
            if(FHexDecBin==0) UnderInfoText.append(QString().sprintf("%-9X",i*WordCounter));
            if(FHexDecBin==1) UnderInfoText.append(QString().sprintf("%-11X",i*WordCounter));
            if(FHexDecBin==2) UnderInfoText.append(QString().sprintf("%-12X",i*WordCounter));
            if(FHexDecBin==3) UnderInfoText.append(QString().sprintf("%-33X",i*WordCounter));
        }
    }
    ui->UnderInfoLine->setText(UnderInfoText);
}

void ArraysView::ShowContextMenu(const QPoint &pos)
{
    ArrViewTimer->stop();
    QPoint viewPos = QCursor::pos();
    QPointF scenePos = ui->TextEditInfo->mapFromGlobal(viewPos);
    if(scenePos.x()>=100)
    {
        QString CurrAddress = ui->AddrLine->text();
        int ClickedAddress=0;
        int fontHeight=ui->TextEditInfo->fontMetrics().height();
        int fontWidth=ui->TextEditInfo->fontMetrics().width('0');
        int LocalX=scenePos.x()-12*fontWidth;
        int LocalY=scenePos.y();
        int step=0;
        int Koeff=1;
        bool ok;
        int number=0;

        step=fontWidth*Kformat;
        if(F8_16_32Param  == 0) Koeff=1;
        else if(F8_16_32Param  == 1) Koeff=2;
        else if (F8_16_32Param  == 2) Koeff=4;
        if(F8_16_32Param  == 0) number=(LocalX/step)*Koeff+(WidthElement*(LocalY/fontHeight)*Koeff);
        if(F8_16_32Param  == 1) number=(LocalX/step)*Koeff+(WidthElement*(LocalY/fontHeight)*Koeff);
        if(F8_16_32Param  == 2) number=(LocalX/step)*Koeff+(WidthElement*(LocalY/fontHeight)*Koeff);

        if((CurrAddress[0]=='0')&&(CurrAddress[1]=='x'))
        {
             ClickedAddress=(CurrAddress).toInt(&ok,16);
        }
        else
        {
            varlistelm_t *RecombAddress = new varlistelm_t;
            bool update=false;
           dbal_fillfromdb(CurrAddress,RecombAddress,update);
           ClickedAddress=RecombAddress->addr;
        }

        if(F8_16_32Param == 0)
        {
             ClickedAddress=ClickedAddress+number+Smeschenie+LeftInterval;
        }
        if(F8_16_32Param == 1)
        {
            ClickedAddress=ClickedAddress+number+Smeschenie*2+LeftInterval*2;
         }
        if(F8_16_32Param == 2)
        {
            ClickedAddress=ClickedAddress+number+Smeschenie*4+LeftInterval*4;
        }

        RewriteAction->SetAddress(ClickedAddress);
        char nnn[20];
        unsigned char chnl;unsigned long addr=(unsigned long)RewriteAction->ChangingAddress;char* namebuf=nnn;
        dbal_addr2name(chnl,addr,namebuf);
        RewriteAction->SetCustomName((QString)nnn);
        RewriteAction->SetParam();

        if(F8_16_32Param  == 0)
        {
            if(FormatCounter == 3)
                RewriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[0-1]{8}"),this));
            if(FormatCounter == 4)
                RewriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[0-9]{3}"),this));
            if(FormatCounter == 9)
                RewriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[-0-9]{3}"),this));
            if(FormatCounter == 20)
                RewriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[A-Fa-f0-9]{2}"),this));
        }
        if(F8_16_32Param  == 1)
                {
                    if(FormatCounter == 5)
                        RewriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[0-1]{16}"),this));
                    if(FormatCounter == 6)
                        RewriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[0-9]{5}"),this));
                    if(FormatCounter == 10)
                        RewriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[-0-9]{5}"),this));
                    if(FormatCounter == 21)
                        RewriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[A-Fa-f0-9]{4}"),this));
                }
         if(F8_16_32Param  == 2)
                        {
                            if(FormatCounter == 7)
                                RewriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[0-1]{32}"),this));
                            if(FormatCounter == 8)
                                RewriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[0-9]{10}"),this));
                            if(FormatCounter == 11)
                                RewriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[-0-9]{10}"),this));
                            if(FormatCounter == 22)
                                RewriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[A-Fa-f0-9]{8}"),this));
                        }
        QPoint globalPos = ui->centralwidget->mapToGlobal(pos);
        ContextMenu->exec(globalPos);
    }
    ArrViewTimer->start();
}

void ArraysView::ContextMenuButtonClicked()
{
    QString NewValue;
    int NewVal=0;
    bool ok;
    signed char Char=0;
    signed short Short=0;
    signed int Int=0;

    NewValue=RewriteAction->CurrenrValueLine->text();
    if((FormatCounter==20)||(FormatCounter==21)||(FormatCounter==22))
    {
            NewVal=NewValue.toUInt(&ok,16);
    }
    if((FormatCounter==3)||(FormatCounter==5)||(FormatCounter==7))
    {
            NewVal=NewValue.toUInt(&ok,2);
    }
    if((FormatCounter==9)||(FormatCounter==10)||(FormatCounter==11))
    {
        if(FormatCounter==9)
              Char=NewValue.toInt(&ok,10);
        if(FormatCounter==10)
              Short=NewValue.toInt(&ok,10);
        if(FormatCounter==11)
              Int=NewValue.toInt(&ok,10);
    }
    if((FormatCounter==4)||(FormatCounter==6)||(FormatCounter==8))
    {
         NewVal=NewValue.toInt(&ok,10);
    }

    if((NewValue[0]=='\0')&&(!(ArrViewSocket->state()==QAbstractSocket::ConnectedState)))
    {
        qDebug()<<"ok";
    }
    else
    {
        if(F8_16_32Param == 0)
        {
              if(FormatCounter == 9)
              {
                  ArrViewSocket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",RewriteAction->ChangingAddress,(signed char)Char));
              }else ArrViewSocket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",RewriteAction->ChangingAddress,NewVal));
        }
        if(F8_16_32Param == 1)
        {
            if(FormatCounter == 10)
            {
                ArrViewSocket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",RewriteAction->ChangingAddress,(signed short)Short));
            }else  ArrViewSocket->write(QString().sprintf("capture \"mwh 0x%X 0x%X\"\x1A",RewriteAction->ChangingAddress,NewVal));
        }
        if(F8_16_32Param == 2)
        {
            if(FormatCounter == 11)
            {
                ArrViewSocket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",RewriteAction->ChangingAddress,(signed int)Int));
            }else ArrViewSocket->write(QString().sprintf("capture \"mww 0x%X 0x%X\"\x1A",RewriteAction->ChangingAddress,NewVal));
        }
    }
    ReadFromSocket();
    ArrViewData.clear();
    ContextMenu->hide();
}
