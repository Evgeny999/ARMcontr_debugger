#include "memorycontenttablecustom.h"

MemoryContentTableCustom::MemoryContentTableCustom(QWidget *parent):
        MemoryContentTable(parent)
{
        qDebug()<<"1111";
        int CurrentAddress=2222222222;
        CurrentAddress=CurrentAddress & 00000011;
        qDebug()<<CurrentAddress;
}


void MemoryContentTableCustom::ChangeInfo()
{
    //qDebug()<<"222";
    //qDebug()<<"data="<<MemoryContentData;
    bool ok;
    QStringList Words= QString(MemoryContentData).split(' ');
    for(int i=0;i<Words.size();i++)
    {
        QString aaa = Words.at(i);
         if(aaa.size()==0) continue;
        int CurrentNum = aaa.toInt(&ok,16);
        //qDebug()<<i<<"---"<<aaa<<"---"<<CurrentNum;
    }
}
