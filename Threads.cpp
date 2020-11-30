#include "Threads.h"


ReadThread::ReadThread()
{
     ReadSocket = new QTcpSocket(this);

}

ReadThread::~ReadThread()
{

}


void ReadThread::run()
{

}
