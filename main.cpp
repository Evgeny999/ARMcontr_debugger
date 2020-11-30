#include <QtGui/QApplication>
#include "mainwindow.h"
#include "alotofwindow.h"
#include "dbal.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("WINDOWS-1251"));
    qDebug()<<QApplication::applicationDirPath();
    //add param name
    dbal_init();
    ALotOfWindow w;
    w.setWindowTitle("ARM-DBG-Ver.7.20");
    if(w.control==1)
    {
        w.deleteLater();
        return 1;
    }else w.show();
    return a.exec();
}
