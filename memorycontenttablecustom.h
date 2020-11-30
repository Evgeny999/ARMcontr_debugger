#ifndef MEMORYCONTENTTABLECUSTOM_H
#define MEMORYCONTENTTABLECUSTOM_H
#include "memorycontenttable.h"


class MemoryContentTableCustom : public MemoryContentTable
{
    Q_OBJECT
public:
    MemoryContentTableCustom(QWidget *parent = 0);
public slots:
void ChangeInfo();

};


#endif // MEMORYCONTENTTABLECUSTOM_H
