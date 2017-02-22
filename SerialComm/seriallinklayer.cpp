#include "seriallinkLayer.h"

#include <QDebug>
#include <QTimer>
#include <windows.h>

#define HEAD1 0xFF
#define HEAD2 0x00
#define HEAD_GUARD 0x00
#define WAIT_HEAD1 0
#define WAIT_HEAD2 1
#define WAIT_LEN 2

#undef qDebug
#define qDebug QT_NO_QDEBUG_MACRO

SerialLinkLayer::SerialLinkLayer(QObject *parent) :
    //SerialPortWrapper(parent)
    QSerialPort(parent)
{
    state=0;
    frameSize=0;
    rxBuffer.clear();
    connect(this,SIGNAL(readyRead()),this,SLOT(readFromSerial()));
}

void SerialLinkLayer::sendFrame(QByteArray data){
    qDebug()<<"SerialLinkLayer:sendFrame="<<data.size();
    QByteArray temp;
    uint8_t size=data.size();
    temp.append((char)HEAD1);
    temp.append((char)HEAD2);
    temp.append((char)size);
    quint8 checksum=0;    
    for(int i=0;i<size;i++){
        checksum+=data[i];
        temp.append(data[i]);
        if( i>0 && (char)HEAD1==data.at(i-1) && (char)HEAD2==data.at(i) ){
            temp.append((char)HEAD_GUARD);
            qDebug()<<"SerialLinkLayer:sendFrame HEAD_GUARD@"<<i;
        }
    }
    temp.append((~checksum));
    write(temp);
}

void SerialLinkLayer::readFromSerial(void)
{
    QByteArray data = readAll();
    //QByteArray data;
    //qDebug()<<"SerialLinkLayer:readAll="<<data.length();
    rxStateMachine(data);
}

void SerialLinkLayer::rxStateMachine(QByteArray &data){
    for(int i=0;i<data.length();i++){
        streamInput(data.at(i));
        //printf("%02X ",(uint8_t)data.at(i));
        fflush(stdout);
    }
}


int SerialLinkLayer::checkHeader(uint8_t data){
	switch(state){
		case WAIT_HEAD1:
			if(data==HEAD1){
				state++;
			}
			break;
		case WAIT_HEAD2:
			if(data==HEAD2){
				state++;
				break;
			}else if(data==HEAD1){
				break;
			}else{
				state=WAIT_HEAD1;
			}
			break;
		case WAIT_LEN:
			if(data==HEAD1){
				state=WAIT_HEAD2;
				return -1;		//pass to linklayer_input to ignore HEAD GUARD 0x00
			}else if(data>0){
                state=WAIT_HEAD1;
				return 1;
			}
			break;
		default:	//none of preState business
			break;
	}
	return 0;
}

int SerialLinkLayer::streamInput(uint8_t data){
    int ret=checkHeader(data);
    if(ret==1){     //new head founded
        rxBuffer.clear();
        frameSize=data;
        checksum=0;
    }else if(ret==0 && frameSize>0){  //jump HEAD_GUARD && rx started
        if(rxBuffer.size() < frameSize){
            rxBuffer.append(data);
            checksum+=data;
        }else if(data == (uint8_t)(~checksum)){
            emit frameReceived(rxBuffer);
            frameSize=0;
        }else{
            frameSize=0;
        }
    }
    return 0;
}

