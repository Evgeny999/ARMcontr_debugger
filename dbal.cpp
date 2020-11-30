//#include "tdef.h"
#include "varlistelm.h"
#include <QStringList>
#include "dbal.h"
#include <QFile>
#include <QMessageBox>
#include <QDebug>

//char dbal_filename[200];

#ifdef WIN32
#define DLLSPEC extern "C" __cdecl
#else
#define DLLSPEC extern "C" __attribute__((visibility("default"))) __attribute__((__cdecl__))
#endif
DLLSPEC bool  dbal__init(const char* FileName);
DLLSPEC void  dbal__done();
DLLSPEC char* dbal__getsuggestedparamlist(const char* parname);
DLLSPEC void  dbal__fillfromdb(const char* parname,varlistelm_t* elm,bool update);
DLLSPEC void  dbal__addr2name(unsigned char chnl,unsigned long addr, char* namebuf);

void dbal_init()
{
    char *dbal_filename = new char[200];
    QFile File("Param.txt");
    QString FileInfo;
    QString  FileName;
    int FileCounter=0;
    int FileNameCounter=0;

    if(File.exists())
    {
        if(File.open(QIODevice::ReadOnly))
        {
            FileInfo = QString(File.readAll());
            while(FileInfo[FileCounter]!='\0')
            {
                if((FileInfo[FileCounter]=='d')&&(FileInfo[FileCounter+1]=='b')&&(FileInfo[FileCounter+2]==':'))
                {
                    FileCounter=FileCounter+3;
                    while(FileInfo[FileCounter]!=';')
                    {
                        FileName[FileNameCounter] = FileInfo[FileCounter];
                        FileNameCounter++;
                        FileCounter++;
                    }
                }
                if(FileInfo[FileCounter]!='\0') FileCounter++;
            }
            FileName.replace(',',';');
            strcpy(dbal_filename,FileName);
            dbal__init(dbal_filename);
        }
    }
    File.close();
}

void dbal_done()
{
dbal__done();
}

void dbal_getsuggestedparamlist(const char* parname,QStringList *parlist)
{
char* ptr=dbal__getsuggestedparamlist(parname);
QString tmpstr(ptr);
//qDebug()<<tmpstr;
//qDebug()<<"______________________________________________";
parlist->append(tmpstr.split(QChar(';')));
}

void dbal_fillfromdb(const char* parname,varlistelm_t* elm,bool update)
{
dbal__fillfromdb(parname,elm,update);
}

void dbal_addr2name(unsigned char chnl,unsigned long addr,char* namebuf)
{
dbal__addr2name(chnl,addr,namebuf);
}
