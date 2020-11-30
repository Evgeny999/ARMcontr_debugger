#ifndef DBAL_H
#define DBAL_H
#include "varlistelm.h"
#include <QStringList>
void dbal_init();
void dbal_done();
void dbal_getsuggestedparamlist(const char* parname,QStringList *parlist);
void dbal_fillfromdb(const char* parname,varlistelm_t* elm,bool update);
void dbal_addr2name(unsigned char chnl,unsigned long addr,char* namebuf);

#endif // DBAL_H
