#ifndef THREADS_H
#define THREADS_H

#include <QThread>
#include <QTimer>
#include <QTcpSocket>

typedef struct{

}request;

extern QList<QString> RequestStack;


class ReadThread : public QThread
{
    Q_OBJECT
public:
    ReadThread();
    ~ReadThread();

    void run();

  QTimer Timer;
  QTcpSocket* ReadSocket;
};

#endif // THREADS_H
