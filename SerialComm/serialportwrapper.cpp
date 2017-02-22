#include "serialportwrapper.h"
#include "serialthread.h"
#include <QDebug>

#undef qDebug
#define qDebug QT_NO_QDEBUG_MACRO

SerialPortWrapper::SerialPortWrapper(QObject *parent):
    QSerialPort(0)
{
    updateFlag=0;
    thread=new SerialThread(this,parent);
}

bool SerialPortWrapper::open(OpenMode mode){
    qDebug()<<"SerialPortWrapper::open";
    Q_UNUSED(mode);
    return thread->open();
}

void SerialPortWrapper::close(){
    qDebug()<<"SerialPortWrapper::close";
    thread->close();
}

void SerialPortWrapper::safe_open(void){
    if(QSerialPort::open(QSerialPort::ReadWrite)==false){
        //this function running in the new thread,qDebug will throw connect error
        qDebug()<<"SerialPortWrapper::safe_open error="<<errorString();
    }
    updateFlag=1;
}

void SerialPortWrapper::safe_close(void){
    qDebug()<<"SerialPortWrapper::safe_close";
    QSerialPort::close();
    updateFlag=1;
}

QByteArray SerialPortWrapper::readAll(){
    QByteArray a;
    return a;
}

qint64 SerialPortWrapper::write(QByteArray &data){
    return 0;
}












