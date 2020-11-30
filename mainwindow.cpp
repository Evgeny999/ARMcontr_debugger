#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "alotofwindow.h"
#include "dbal.h"
int DellSize=4312;
int Byte8=761;
int KAdr=384;
//QString Address;
int ClickedAddress;
//Change Ip/Port
ParamStruct Quantiti[15];
//FontParam
const float MemoryWindow_FONT_FORMAT=1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    counter = 0;
    FormatCounter=20;
    Kformat=3;
    ControlRepeat=0;
    InfoSize=0;
    WidthElement=0;
    HeightElement=0;
    He=0;
    Wi=0;
    number=300;
    ui->setupUi(this);
    ui->centralWidget->setFocusPolicy(Qt::StrongFocus);
    Address="0x10";
    ui->lineEdit->setText(Address);
    ui->textEdit->verticalScrollBar()->hide();
    //ComcoCox1
    ui->comboBox->addItem("8",QVariant(8));
    ui->comboBox->addItem("16",QVariant(16));
    ui->comboBox->addItem("32",QVariant(32));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(SelectbhwFormat(int)));
    ui->comboBox->setCurrentIndex(2);
    //Combobox
    ui->comboBox_2->addItem("hex",QVariant(3));//3hex
    ui->comboBox_2->addItem("dec",QVariant(2));//2dec Not Znak
    ui->comboBox_2->addItem("decSign",QVariant(4));//Znak
    ui->comboBox_2->addItem("bin",QVariant(1));//1bin
    connect(ui->comboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(SelectHDBFormat(int)));
    //ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9}_{*]{10}"),this));
    //AddIpPortCh (ComboBox 3)
    GetParamToComboBox();
    //
    socket = new QTcpSocket(this);
    tmr = new QTimer(this);
    timedelay=500;
    SetTimerInterval();
    connect(tmr,SIGNAL(timeout()),this,SLOT(Connect()));

    ui->textEdit->setEnabled(true);
    ui->textEdit->setReadOnly(true);
    ReWriteAction = new ReWriteContMenuWidget();
    ReWriteAction->SaveToBufferText=ui->textEdit;
    ui->textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->textEdit,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(ShowContextMenu(const QPoint&)));
    ContextMenu = new QMenu(this);
    ContextMenu->addAction(ReWriteAction);
    connect(ReWriteAction->ChangeButton,SIGNAL(clicked()),this,SLOT(ContextMenuButtonClicked()));


    this->menuBar();
    menuBar()->addAction("|Частота обновления->");
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

    int ComboBoWidth =ui->comboBox_3->fontMetrics().width('0')*10;
    ui->comboBox_3->setFixedWidth(ComboBoWidth);

    tmr->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetDelay05Sec()
{
    timedelay = 500;
    SetTimerInterval();
}
void MainWindow::SetDelay1Sec()
{
    timedelay = 1000;
    SetTimerInterval();
}
void MainWindow::SetDelay3Sec()
{
    timedelay = 3000;
    SetTimerInterval();
}
void MainWindow::SetDelay5Sec()
{
    timedelay = 5000;
    SetTimerInterval();
}
void MainWindow::SetTimerInterval()
{
    tmr->setInterval(timedelay);
}


void MainWindow::Connect()
{
        tmr->stop();
        bool ok1;
        QByteArray ChannelCheck;
        int Check;
        bool ReadyToWork=false;
        varlistelm_t *RecombAddress = new varlistelm_t;
        Address=ui->lineEdit->text();
        if((Address[0]=='0')&&(Address[1]=='x'))
        {
            SetAddress=(Address).toInt(&ok1,16);
        }
        else
        {
            bool update=false;
           dbal_fillfromdb(Address,RecombAddress,update);
           SetAddress=RecombAddress->addr;
        }
        //Size
        Size();
        LocalInfoSize = InfoSize;
        LocalWidthElement = WidthElement;
        LocalHeightElement = HeightElement;

        if(socket->state()==QAbstractSocket::ConnectedState)
        {
            if (socket->isOpen())
            {
                        tmr->stop();
               if(NymberOfChennalControl>1)//если заданных каналов в подключении больше чем 1, то нужно выбрать канал исходя из параметров
               {
                        bool CheackSearch=false;
                        socket->write(QString().sprintf("capture \"pcilink_setchan %d\"\x1A",CHANNAL));
                        socket->waitForBytesWritten();
                        QByteArray BBB;
                        while(ChannelCheck.size()<=13)
                        {
                            if(socket->waitForReadyRead(5000)){}
                            else
                            {
                                socket->disconnectFromHost();
                                tmr->start();
                                break;
                            }
                            BBB=socket->peek(socket->bytesAvailable());
                            for(int i=0;i<BBB.size();i=i+1)
                            {
                                if(BBB[i]=='\x1A')
                                {
                                    ChannelCheck=socket->read(i=i+1);
                                    CheackSearch=true;
                                    break;
                                }
                            }
                            if(CheackSearch==true) break;
                            BBB.clear();
                        }

                        for(int i=0;i<=ChannelCheck.size();i++)
                        {
                            if((ChannelCheck[i]=='c')&&(ChannelCheck[i+1]=='h')&&(ChannelCheck[i+2]=='a')&&(ChannelCheck[i+3]=='n')&&(ChannelCheck[i+4]=='n')&&(ChannelCheck[i+5]=='e')&&(ChannelCheck[i+6]=='l')&&(ChannelCheck[i+7]==' ')&&(ChannelCheck[i+8]=='=')&&(ChannelCheck[i+9]==' '))
                            {
                                i=i+10;
                                Check=QString::number(ChannelCheck[i]).toInt() - 48;
                                if(Check==CHANNAL)   ReadyToWork=true;
                            }
                        }
                         //end set channel
                }else ReadyToWork=true;



                        if((ReadyToWork==true)&&(socket->state()==QAbstractSocket::ConnectedState))
                        {
                         if (counter == 0)
                             {
                             //socket->write(QString("capture \"mdb 0x%0 %1\"\x1A").arg(SetAddress).arg(number));
                              socket->write(QString().sprintf("capture \"mdb 0x%X %d\"\x1A",SetAddress,number));
                              //socket->waitForBytesWritten(5000);
                             }
                         if (counter == 1)
                             {
                             //socket->write(QString("capture \"mdh 0x%0 %1\"\x1A").arg(SetAddress).arg(number));
                             socket->write(QString().sprintf("capture \"mdh 0x%X %d\"\x1A",SetAddress,number));
                            // socket->waitForBytesWritten(5000);
                             }
                         if (counter == 2)
                             {
                             //socket->write(QString("capture \"mdw 0x%0 %1\"\x1A").arg(SetAddress).arg(number));
                             socket->write(QString().sprintf("capture \"mdw 0x%X %d\"\x1A",SetAddress,number));
                             //socket->waitForBytesWritten(5000);
                             }
                           }
                          sockReady(LocalInfoSize,LocalWidthElement,LocalHeightElement);
                }
                else
                {
                     ui->textEdit->setText("Do not open socket");
                }
        }
        else
        {
          socket->connectToHost(QString("%0").arg(IP),PORT);
          socket->waitForConnected(10);
          ui->textEdit->setText("Not connected.");
          tmr->start();
        }
}

void MainWindow::sockDisc()
{
    socket->deleteLater();
}
void MainWindow::sockReady(int LocalInfoSize,int LocalWidthEl,int LocalHeightEl)
{

    if(socket->state()==QAbstractSocket::ConnectedState)
            {
               Data.clear();
               DellData.clear();
               bool CheckSearch=false;
               QByteArray DataPeek;
               while(Data.size()<LocalInfoSize*200)
               {
                   //socket->waitForReadyRead(100);
                   if(socket->waitForReadyRead(5000)){}
                   else
                   {
                       socket->disconnectFromHost();
                       tmr->start();
                       break;
                   }
                   DataPeek=socket->peek(LocalInfoSize);
                   for(int i=0;i<DataPeek.size();i=i+1)
                   {
                       if(DataPeek[i]=='\x1A')
                       {
                           Data.append(socket->read(i=i+1));
                           CheckSearch=true;
                           break;
                       }
                   }
                   if(CheckSearch==true)
                   {
                       break;
                   }
                   else(Data.append(socket->read(DataPeek.size())));
                   DataPeek.clear();
               }
             if(Data.size()!=0)
                {
                     Filter();
                     AddLineUnderInfo();
                     ui->textEdit->setText(DellData);
                     DellData.clear();
                     DellData.clear();
                     Data.clear();
                     tmr->start();
                     ControlRepeat=0;
                }
                else
                {
                    Data.clear();
                    DellData.clear();
                    tmr->start();
                    ControlRepeat=1;
                }
            }
}

void MainWindow::AddLineUnderInfo()
{
    int F8_16_32 = ui->comboBox->currentIndex();
    int FHexDecBin = ui->comboBox_2->currentIndex();
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

    ui->InfoUnderLine->setText(UnderInfoText);
}

//Format Hex Dec Bin -----------------------------------
void MainWindow::SelectHDBFormat(int index)
{
    int value=ui->comboBox_2->itemData(index).toInt();
    if(value == 1)
    {
        if(counter == 0)
        {
          setbin();
        }
      if(counter==1)
        {
        set16bin();
        }
      if(counter==2)
      {
          set32bin();
      }
    }
    if(value == 2)
    {
        if(counter == 0)
        {
         setdec();
        }
        if(counter == 1)
        {
            set16dec();
        }
        if(counter == 2)
        {
            set32dec();
        }
    }
    if(value == 3)
    {
        if(counter == 0)
        {
         sethex();
        }
        if(counter == 1)
        {
            set16hex();
        }
        if(counter == 2)
        {
            set32hex();
        }
    }
    if(value == 4)
    {
        if(counter == 0)
        {
            setdec2();
        }
        if(counter == 1)
        {
            set16dec2();
        }
        if(counter  == 2)
        {
            set32dec2();
        }
    }
}


void MainWindow::setbin()
{
    FormatCounter = 3;
    Kformat=9;
}
void MainWindow::setdec()
{
    FormatCounter = 4;
    Kformat=4;
}
void MainWindow::setdec2()
{
    FormatCounter = 9;
    Kformat = 5;
}

void MainWindow::sethex()
{
    FormatCounter = 20;
    Kformat=3;
}

void MainWindow::set16bin()
{
    FormatCounter = 5;
    Kformat = 17;
}
void MainWindow::set16dec()
{
    FormatCounter = 6;
    Kformat = 6;
}
void MainWindow::set16dec2()
{
    FormatCounter = 10;
    Kformat = 7;
}

void MainWindow::set16hex()
{
    FormatCounter = 21;
    Kformat = 5;
}
void MainWindow::set32bin()
{
    FormatCounter = 7;
    Kformat = 33;
}
void MainWindow::set32dec()
{
    FormatCounter = 8;
    Kformat = 11;
}
void MainWindow::set32dec2()
{
    FormatCounter = 11;
    Kformat = 12;
}

void MainWindow::set32hex()
{
    FormatCounter = 22;
    Kformat = 9;
}



//Format 8 16 32-----------------------------------------
void MainWindow::SelectbhwFormat(int index)
{
    int value=ui->comboBox->itemData(index).toInt();
    if(value == 8)
    {
      setb();
      //set param in hex if changed
      ui->comboBox_2->setCurrentIndex(0);
      FormatCounter = 20;
    }
    if(value == 16)
    {
      seth();
      ui->comboBox_2->setCurrentIndex(0);
      FormatCounter = 21;
    }
    if(value == 32)
    {
      setw();
      ui->comboBox_2->setCurrentIndex(0);
      FormatCounter = 22;
    }


}

void MainWindow::setb()
{
    counter = 0;
    Kformat=3;
}
void MainWindow::seth()
{
    counter = 1;
    Kformat=5;
}
void MainWindow::setw()
{
    counter = 2;
    Kformat=9;
}
//----------------------------------------


void MainWindow::Size()
{
    //Определяем размеры соот ветствующие разным форматам вывода информации
    Width=ui->textEdit->width();
    Height=ui->textEdit->height();
    int ElemHeight = ui->textEdit->fontMetrics().height();
    int ElemWidth = ui->textEdit->fontMetrics().width('0');
    //Wi=((Width-30-96)/(8*Kformat))*MemoryWindow_FONT_FORMAT;
     Wi=((Width-30-12*ElemWidth)/(ElemWidth*Kformat))*MemoryWindow_FONT_FORMAT;//New
     WidthElement=floor(Wi);//Width - otstup - add/ Pwight*NumOfElement
     //He=(Height)/16;
     He=(Height)/ElemHeight; //NEw
     HeightElement=floor(He);
     //Param out
     number=WidthElement*HeightElement;
     //qDebug()<<number;
     InfoSize=HeightElement*(12+WidthElement*Kformat);
}

void MainWindow::Filter()
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

    LocalData=QString(Data);
    Data.clear();

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
     if((FormatCounter == 4)&&(counter==0))
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
     if((FormatCounter == 6)&&(counter==1))
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
     if((FormatCounter == 8)&&(counter==2))
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
       if((FormatCounter == 9)&&(counter==0))
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
       if((FormatCounter == 10)&&(counter==1))
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
       if((FormatCounter == 11)&&(counter==2))
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
     if ((FormatCounter == 3)&&(counter == 0))
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
     if ((FormatCounter == 5)&&(counter == 1))
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
     if ((FormatCounter == 7)&&(counter == 2))
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


     //Set Bin-----------------------------
     //----------------------------------------------------------------------------------------------------------------------------------------------------------------------
   if((Address[0]=='0')&&(Address[1]=='x'))
     {
       LocalAddress=(Address).toInt(&ok,16);
     }
   else
   {
       varlistelm_t *RecombAddress = new varlistelm_t;
       bool update=false;
      dbal_fillfromdb(Address,RecombAddress,update);
      LocalAddress=RecombAddress->addr;
   }
   //Filtet and Set Address
           int LocCoun=0;
           DellData.insert(0,QString().sprintf("0x%08X: ",LocalAddress));
           if(counter == 0)
           {
           LocalAddress=LocalAddress+WidthElement;
           }
           if(counter == 1)
           {
           LocalAddress=LocalAddress+WidthElement*2;
            }
           if(counter == 2)
           {
           LocalAddress=LocalAddress+WidthElement*4;
            }
           //qDebug()<<WidthElement<<"<---WidthElement";
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
                   if(counter == 0)
                   {
                   LocalAddress=LocalAddress+WidthElement;
                   }
                   if(counter == 1)
                   {
                   LocalAddress=LocalAddress+WidthElement*2;
                    }
                   if(counter == 2)
                   {
                   LocalAddress=LocalAddress+WidthElement*4;
                    }
               }

           }


           /*
           for(int i=0;i<HeightElement;i++)
           {
               for(int j=0;j<=WidthElement*Kformat;j++)
                   if(j==0)
                   {
                    DellData.insert(LocCoun,QString().sprintf("0x%08X: ",LocalAddress));
                    LocCoun=LocCoun+12;
                   }
                    else LocCoun++;
               LocalAddress=LocalAddress+WidthElement;
           }*/

}

void MainWindow::on_lineEdit_editingFinished()
{
    Address=ui->lineEdit->text();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
        bool ok;
        Address=ui->lineEdit->text();
        int SetAddr;
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
        if(event->key() == Qt::Key_PageDown)
        {
            if(counter == 0)
            {
            SetAddr=SetAddr+WidthElement;
            }
            if(counter == 1)
            {
            SetAddr=SetAddr+WidthElement*2;
             }
            if(counter == 2)
            {
            SetAddr=SetAddr+WidthElement*4;
             }
            ui->lineEdit->setText(QString().sprintf("0x%08X",SetAddr));

        }
        if(event->key() == Qt::Key_PageUp)
        {
            if(SetAddr-WidthElement>=0)
            {
                if(counter == 0)
                {
                SetAddr=SetAddr-WidthElement;
                }
                if(counter == 1)
                {
                SetAddr=SetAddr-WidthElement*2;
                 }
                if(counter == 2)
                {
                SetAddr=SetAddr-WidthElement*4;
                }
                ui->lineEdit->setText(QString().sprintf("0x%08X",SetAddr));
             }
        }
}

//------------------------------------Resert Address-------------------------------------
void MainWindow::ShowContextMenu(const QPoint &pos)
{
    tmr->stop();
    QPoint viewPos = QCursor::pos();
    QPointF scenePos = ui->textEdit->mapFromGlobal(viewPos);
    if(scenePos.x()>=100)
    {
        int fontHeight=ui->textEdit->fontMetrics().height();
        int fontWidth=ui->textEdit->fontMetrics().width('0');
        int LocalX=scenePos.x()-12*fontWidth;
        int LocalY=scenePos.y();
        int step=0;
        int Koeff=1;
        bool ok;

        step=fontWidth*Kformat;
        if(counter == 0) Koeff=1;
        else if(counter == 1) Koeff=2;
        else if (counter == 2) Koeff=4;
        if(counter == 0) number=(LocalX/step)*Koeff+(WidthElement*(LocalY/fontHeight)*Koeff);
        if(counter == 1) number=(LocalX/step)*Koeff+(WidthElement*(LocalY/fontHeight)*Koeff);
        if(counter == 2) number=(LocalX/step)*Koeff+(WidthElement*(LocalY/fontHeight)*Koeff);
        if((Address[0]=='0')&&(Address[1]=='x'))
        {
             ClickedAddress=(Address).toInt(&ok,16);
        }
        else
        {
            varlistelm_t *RecombAddress = new varlistelm_t;
            bool update=false;
           dbal_fillfromdb(Address,RecombAddress,update);
           ClickedAddress=RecombAddress->addr;
        }

        ClickedAddress=ClickedAddress+number;
        ReWriteAction->SetAddress(ClickedAddress);
        char nnn[20];
        unsigned char chnl;unsigned long addr=(unsigned long)ReWriteAction->ChangingAddress;char* namebuf=nnn;
        dbal_addr2name(chnl,addr,namebuf);
        ReWriteAction->SetCustomName((QString)nnn);;
        ReWriteAction->SetParam();

        if(counter == 0)
        {
            if(FormatCounter == 3)
                ReWriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[0-1]{8}"),this));
            if(FormatCounter == 4)
                ReWriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[0-9]{3}"),this));
            if(FormatCounter == 9)
                ReWriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[-0-9]{3}"),this));
            if(FormatCounter == 20)
                ReWriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[A-Fa-f0-9]{2}"),this));
        }
        if(counter == 1)
                {
                    if(FormatCounter == 5)
                        ReWriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[0-1]{16}"),this));
                    if(FormatCounter == 6)
                        ReWriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[0-9]{5}"),this));
                    if(FormatCounter == 10)
                        ReWriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[-0-9]{5}"),this));
                    if(FormatCounter == 21)
                        ReWriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[A-Fa-f0-9]{4}"),this));
                }
         if(counter == 2)
                        {
                            if(FormatCounter == 7)
                                ReWriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[0-1]{32}"),this));
                            if(FormatCounter == 8)
                                ReWriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[0-9]{10}"),this));
                            if(FormatCounter == 11)
                                ReWriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[-0-9]{10}"),this));
                            if(FormatCounter == 22)
                                ReWriteAction->CurrenrValueLine->setValidator(new QRegExpValidator(QRegExp("[A-Fa-f0-9]{8}"),this));
                        }
        QPoint globalPos = ui->centralWidget->mapToGlobal(pos);
        ContextMenu->exec(globalPos);
    }
    tmr->start();
}

void MainWindow::ContextMenuButtonClicked()
{
    tmr->stop();
    QString NewValue;
    int NewVal=0;
    bool ok;
    signed char Char=0;
    signed short Short=0;
    signed int Int=0;

    NewValue=ReWriteAction->CurrenrValueLine->text();
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

    if((NewValue[0]=='\0')&&(!(socket->state()==QAbstractSocket::ConnectedState)))
    {
        qDebug()<<"ok";
    }
    else
    {
        if(counter == 0)
        {
              if(FormatCounter == 9)
              {
                  socket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",ReWriteAction->ChangingAddress,(signed char)Char));
              }else socket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",ReWriteAction->ChangingAddress,NewVal));
        }
        if(counter == 1)
        {
            if(FormatCounter == 10)
            {
                socket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",ReWriteAction->ChangingAddress,(signed short)Short));
            }else  socket->write(QString().sprintf("capture \"mwh 0x%X 0x%X\"\x1A",ReWriteAction->ChangingAddress,NewVal));
        }
        if(counter == 2)
        {
            if(FormatCounter == 11)
            {
                socket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",ReWriteAction->ChangingAddress,(signed int)Int));
            }else socket->write(QString().sprintf("capture \"mww 0x%X 0x%X\"\x1A",ReWriteAction->ChangingAddress,NewVal));
        }
    }

    QByteArray BBB;
    QByteArray ChannelCheck;
    bool CheackSearch=false;
    while((socket->bytesAvailable())||(socket->waitForReadyRead(1000)))
    {
        socket->waitForReadyRead(100);
        BBB=socket->peek(socket->bytesAvailable());
        for(int i=0;i<BBB.size();i++)
        {
            if(BBB[i]=='\x1A')
            {
                ChannelCheck=socket->read(i+1);
                CheackSearch=true;
                break;
            }
        }
        if(CheackSearch==true) break;
        BBB.clear();
    }
    ContextMenu->hide();
    tmr->start();
}



/*void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    int x=event->x();
    int y=event->y();
    int LocalX=0;
    int LocalY=0;
    int step=0;
    int number=0;
    bool ok;
    int Koeff=1;
    int fontHeight=ui->textEdit->fontMetrics().height();
    int fontWidth=ui->textEdit->fontMetrics().width('0');
    if(x>=100)
    {
        LocalX=x-12*fontWidth;
        LocalY=y-3*fontHeight;
        //step=8*Kformat;
        step=fontWidth*Kformat;
        if(counter == 0) Koeff=1;
        else if(counter == 1) Koeff=2;
        else if (counter == 2) Koeff=4;
        if(counter == 0) number=(LocalX/step+1)*Koeff+(WidthElement*(LocalY/fontHeight)*Koeff);
        if(counter == 1) number=(LocalX/step+1)*Koeff+(WidthElement*(LocalY/fontHeight)*Koeff)-1;
        if(counter == 2) number=(LocalX/step+1)*Koeff+(WidthElement*(LocalY/fontHeight)*Koeff)-3;
        //ClickedAddress=(Address).toInt(&ok,16);
        if((Address[0]=='0')&&(Address[1]=='x'))
        {
             ClickedAddress=(Address).toInt(&ok,16);
        }
        else
        {
            varlistelm_t *RecombAddress = new varlistelm_t;
            bool update=false;
           dbal_fillfromdb(Address,RecombAddress,update);
           ClickedAddress=RecombAddress->addr;
        }
        ClickedAddress=ClickedAddress+number-1;
        //qDebug()<<QString().sprintf("0x%08X",(ClickedAddress))<<"="<<number;
        ResetAddress(ClickedAddress);
    }
}*/

//--------------------------if double clicked. Create "setAddr" window------------------------------------------------------------
/*void MainWindow::ResetAddress(int Addr)
{
    tmr->stop();
    SetWindow = new setAddWindow(this);
    //input limit
    if(counter == 0)
    {
        if(FormatCounter == 3)
            SetWindow->ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[0-1]{8}"),this));
        if(FormatCounter == 4)
            SetWindow->ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]{3}"),this));
        if(FormatCounter == 9)
            SetWindow->ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[-0-9]{3}"),this));
        if(FormatCounter == 20)
            SetWindow->ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Fa-f0-9]{2}"),this));
    }

            if(counter == 1)
            {
                if(FormatCounter == 5)
                    SetWindow->ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[0-1]{16}"),this));
                if(FormatCounter == 6)
                    SetWindow->ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]{5}"),this));
                if(FormatCounter == 10)
                    SetWindow->ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[-0-9]{5}"),this));
                if(FormatCounter == 21)
                    SetWindow->ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Fa-f0-9]{4}"),this));
            }
                    if(counter == 2)
                    {
                        if(FormatCounter == 7)
                            SetWindow->ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[0-1]{32}"),this));
                        if(FormatCounter == 8)
                            SetWindow->ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]{10}"),this));
                        if(FormatCounter == 11)
                            SetWindow->ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[-0-9]{10}"),this));
                        if(FormatCounter == 22)
                            SetWindow->ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Fa-f0-9]{8}"),this));
                    }
    //input limit
    SetWindow->ui->label_3->setText(QString().sprintf("0x%08X: ",Addr));
    SetWindow->ui->lineEdit->setText("");
    SetWindow->show();
    SetWindow->counter=counter;
    SetWindow->FormatCounter=FormatCounter;
    SetWindow->socket=socket;
    SetWindow->setWindowTitle("Reset");
    SetWindow->tmr=tmr;
}*/

//Start timer when window hide
/*void setAddWindow::on_pushButton_clicked()
{
    QString NewValue;
    int NewVal=0;
    bool ok;
    signed char Char=0;
    signed short Short=0;
    signed int Int=0;

    NewValue=ui->lineEdit->text();
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
    if((NewValue[0]=='\0')&&(!(socket->state()==QAbstractSocket::ConnectedState)))
    {
        qDebug()<<"ok";
    }
    else
    {
        if(counter == 0)
        {

              if(FormatCounter == 9)
              {
                  socket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",ClickedAddress,(signed char)Char));
              }else socket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",ClickedAddress,NewVal));
        }
        if(counter == 1)
        {
            if(FormatCounter == 10)
            {
                socket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",ClickedAddress,(signed short)Short));
            }else  socket->write(QString().sprintf("capture \"mwh 0x%X 0x%X\"\x1A",ClickedAddress,NewVal));
        }
        if(counter == 2)
        {
            if(FormatCounter == 11)
            {
                socket->write(QString().sprintf("capture \"mwb 0x%X 0x%X\"\x1A",ClickedAddress,(signed int)Int));
            }else socket->write(QString().sprintf("capture \"mww 0x%X 0x%X\"\x1A",ClickedAddress,NewVal));
        }
    }
    hide();
    QByteArray BBB;
    QByteArray ChannelCheck;
    bool CheackSearch=false;
    while((socket->bytesAvailable())||(socket->waitForReadyRead(1000)))
    {
        socket->waitForReadyRead(100);
        Sleep(0);
        BBB=socket->peek(100);
        for(int i=0;i<BBB.size();i++)
        {
            if(BBB[i]=='\x1A')
            {
                ChannelCheck=socket->read(i+1);
                CheackSearch=true;
                break;
            }
        }
        if(CheackSearch==true) break;
        BBB.clear();
    }
    tmr->start();
}

//Start timer when window close
void setAddWindow::closeEvent(QCloseEvent *e)
{
     tmr->start();
     e->accept();
}*/
//--------------------------------------------ResetAddress----------------------------------------------------


void MainWindow::closeEvent(QCloseEvent *e)
{
    tmr->stop();
    socket->deleteLater();
    //e->accept();
}

//---------------------------------------------------------------------------
//Add IpAddr Port and chann
//----------------------------------------------------------------------------
void ALotOfWindow::ReadFile()
{
    IPPort->clear();
    bool ok;
    int i=0;
    int Num=0;
    QString LocalPort;
    QString Name;
    int counter=0;
    QFile File("Param.txt");

    if(File.exists())
    {
        if(File.open(QIODevice::ReadOnly))
        {
            //ErrorWindow->ui->label->setText("State:FileOpen!!");
            //ErrorWindow->ui->label_2->setText("ParamSuccessfullyAdded");
            //ErrorWindow->show();
            FileInfo=File.readAll();
            int LocalVirtualCounter=0;
            while(FileInfo[i]!='\0')
            {
                //IpSearch
                if((FileInfo[i]=='i')&&(FileInfo[i+1]=='p')&&(FileInfo[i+2]==':'))
                {
                  i=i+3;
                  while(FileInfo[i]!=' ')
                  {
                      Quantiti[counter].IpAddr[Num]=FileInfo[i];
                      i++;
                      Num++;
                  }
                  Num=0;
                  while(FileInfo[i]!=';')
                  {
                      //PortSearch
                      if((FileInfo[i]=='p')&&(FileInfo[i+1]=='o')&&(FileInfo[i+2]=='r')&&(FileInfo[i+3]=='t')&&(FileInfo[i+4]==':'))
                      {
                          i=i+5;
                          while(FileInfo[i]!=' ')
                          {

                              LocalPort[Num]=FileInfo[i];
                              Num++;
                              i++;
                          }
                          Quantiti[counter].Port=LocalPort.toInt(&ok,10);
                          LocalPort.clear();
                          Num=0;
                       }
                      //ChannalSearch
                      if((FileInfo[i]=='c')&&(FileInfo[i+1]=='h')&&(FileInfo[i+2]=='a')&&(FileInfo[i+3]=='n')&&(FileInfo[i+4]=='n')&&(FileInfo[i+5]=='a')&&(FileInfo[i+6]=='l')&&(FileInfo[i+7]==':'))
                      {
                          i=i+8;
                          while(FileInfo[i]!=' ')
                          {

                              LocalPort[Num]=FileInfo[i];
                              Num++;
                              i++;
                          }
                          Quantiti[counter].NumberOfChannal=LocalPort.toInt(&ok,10);
                          LocalPort.clear();
                          Num=0;
                          Quantiti[counter].Flag=1;
                          Quantiti[counter].VirtualCounter=LocalVirtualCounter;
                          LocalVirtualCounter++;
                       }
                      if((FileInfo[i]=='n')&&(FileInfo[i+1]=='a')&&(FileInfo[i+2]=='m')&&(FileInfo[i+3]=='e')&&(FileInfo[i+4]==':'))
                      {
                          QString LocalCostomName;
                          i=i+5;
                         while(FileInfo[i]!=';')
                          {
                             LocalCostomName[Num]=FileInfo[i];
                             Num++;
                             i++;
                          }
                         Quantiti[counter].CustomName=LocalCostomName;
                         Num=0;
                      }
                      if(FileInfo[i]==';')
                      {
                      }
                      else i++;
                   }
                  counter++;
                }
                i++;
            }
            Quantiti[counter].Flag=0;
            //ErrorWindow->ui->textEdit->append(QString("(IP|Port|Chan|Name)\n"));
            QString AddInfoForLabel;
            AddInfoForLabel.append("INFO: \n");
            AddInfoForLabel.append("___________________________________________________________\n");
            AddInfoForLabel.append(QString("              IP              |   Port  |Chan|Name\n"));
            for(int j=0;j<counter;j++)
            {
               //Crete Action with Ip/Port
               //qDebug()<<"-----Cahannal"+QString::number(j);
               // qDebug()<<Quantiti[j].IpAddr<<"---Ip";
               // qDebug()<<QString::number(Quantiti[j].Port)<<"---Port";
               // qDebug()<<QString::number(Quantiti[j].NumberOfChannal)<<"---NumOfChan";
               // qDebug()<<QString::number(Quantiti[j].Flag);
               // qDebug()<<QString::number(Quantiti[j].VirtualCounter);
                //ErrorWindow->ui->textEdit->append(QString("(%0.)%1|%2|%3|%4\n").arg(j).arg(Quantiti[j].IpAddr).arg(QString::number(Quantiti[j].Port)).arg(QString::number(Quantiti[j].NumberOfChannal)).arg(Quantiti[j].CustomName));
                AddInfoForLabel.append(QString("(%0.)%1  |  %2  |  %3    |  %4\n").arg(j).arg(Quantiti[j].IpAddr).arg(QString::number(Quantiti[j].Port)).arg(QString::number(Quantiti[j].NumberOfChannal)).arg(Quantiti[j].CustomName));
                Name=QString("IP:%1  ,Port:%2  ,NumberOF_Channel:%3 , Name:%4").arg(Quantiti[j].IpAddr).arg(QString::number(Quantiti[j].Port)).arg(QString::number(Quantiti[j].NumberOfChannal)).arg(Quantiti[j].CustomName);
                IpPort = new QAction(tr(Name),this);
                IPPort->addAction(IpPort);
            }
            AddInfoForLabel.append("___________________________________________________________\n");
            AddInfoForLabel.append("Подробную информацию об отлачике можно получить в файле \"РуководствоПоИспользованию_ARM_Отладчика\"\n");
            AddInfoForLabel.append(QString("\n\nФормат float добавлен в окнах CellList и MemoryContent(Table)\n"));
            //AddInfoForLabel.append("db: "+dbDirAndName);
            InfoLabel->setText(AddInfoForLabel);
            InfoLabel->setAlignment(Qt::AlignBottom);
        }
        else
        {
            QMessageBox::critical(this,"No access to reading","File found,but could not be read");
            control = 1;
        }
    }
    else
    {
        QMessageBox::critical(this,"DoNotSearchFile",QString("Create \"Param.txt\" in\n %0").arg(QApplication::applicationDirPath()));
        control = 1;
    }
    File.close();
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  Расписать строку с именами в некоторый массив строк который будет записываться в каждыйканал
 */
void MainWindow::GetParamToComboBox()
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
    ui->comboBox_3->addItem("ConnectionSetting");
    ui->comboBox_3->addItem("____________________________________");
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
            ui->comboBox_3->addItem(Name,QVariant(NameCounter));
            NameCounter++;
        }
        i++;
        ui->comboBox_3->addItem("------------------------------------------------------");
    }
    qobject_cast<QListView*>(ui->comboBox_3->view())->setMinimumWidth(300);
    ui->comboBox_3->setEditable(true);
    ui->comboBox_3->lineEdit()->setReadOnly(true);
    ui->comboBox_3->lineEdit()->setAlignment(Qt::AlignRight);
    connect(ui->comboBox_3,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeParam(int)));
}

void MainWindow::ChangeParam(int index)
{
    IP.clear();
    PORT=0;
    int i=0;
    int C1=0;
    QString Par;
    QString PortChanPr;
    int value=ui->comboBox_3->itemData(index).toInt();
    NymberOfChennalControl=NymberOfChConrolMass[value];//Определяем количество каналов, чтобы исключить переключение, если подключено 1 устройство
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
    }

    //qDebug()<<IP;
    //qDebug()<<QString::number(PORT);
    //qDebug()<<QString::number(CHANNAL);
    ControlRepeat=0;
    socket->disconnectFromHost();
}

void MainWindow::on_lineEdit_textEdited(QString)
{
    QStringList CurrentNameList;
    QString text= ui->lineEdit->text();
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
    ui->lineEdit->setCompleter(Completer);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    int SetAddr;
    int Rez=event->delta()/120;
    bool ok;
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
        if(counter == 0)
        {
        SetAddr=SetAddr+WidthElement;
        }
        if(counter == 1)
        {
        SetAddr=SetAddr+WidthElement*2;
         }
        if(counter == 2)
        {
        SetAddr=SetAddr+WidthElement*4;
         }
        ui->lineEdit->setText(QString().sprintf("0x%08X",SetAddr));
    }
    else
    {
        if(counter == 0)
        {
        SetAddr=SetAddr-WidthElement;
        }
        if(counter == 1)
        {
        SetAddr=SetAddr-WidthElement*2;
         }
        if(counter == 2)
        {
        SetAddr=SetAddr-WidthElement*4;
         }
        ui->lineEdit->setText(QString().sprintf("0x%08X",SetAddr));
    }
}
