#include "serialthread.h"
#include "serialportwrapper.h"
#include <QDebug>

#undef qDebug
#define qDebug QT_NO_QDEBUG_MACRO

SerialThread::SerialThread(SerialPortWrapper *serial,QObject *parent) :
    QThread(parent)
{
    this->serial=serial;
    serial->moveToThread(this);
    connect(this,SIGNAL(opening()),serial,SLOT(safe_open()));
    connect(this,SIGNAL(closing()),serial,SLOT(safe_close()));
    //connect(this,SIGNAL(destroyed()),this,SLOT(cleanup()));
    start();
}

bool SerialThread::open(void){
    qDebug()<<"SerialThread::open";
    serial->updateFlag=0;
    emit opening();
    while(serial->updateFlag==0){;}
    return serial->isOpen();
}

bool SerialThread::close(void){
    qDebug()<<"SerialThread::close";
    serial->updateFlag=0;
    emit closing();
    while(serial->updateFlag==0){;}
    return serial->isOpen();
}

void SerialThread::cleanup(void){
    qDebug()<<"SerialThread:cleanup";
    quit();
    wait();
}
SerialThread::~SerialThread(){
    cleanup();
}
