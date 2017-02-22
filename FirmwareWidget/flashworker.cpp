#include "flashworker.h"
#include <QDebug>
//#include <QTextBlock>
//#include <QTextCursor>

#define SECTOR_WRITE_BLOCK 128

#define MAX_RETRY 3
#define MAX_REQUERY 3
#define ISP_CMD_OK 0
#define ISP_CMD_NOK -1
#define ISP_CMD_TO -2
#define ISP_CMD_FATAL -3

using namespace FirmwareProtocol;

FlashWorker::FlashWorker(QObject *parent) :
    QThread(parent)
{
    //qRegisterMetaType<QTextBlock>("QTextBlock");
    //qRegisterMetaType<QTextCursor>("QTextCursor");
    status_reset();
}

void FlashWorker::status_reset(){
    dataLp=0;
    dataSector=4;
    errorCounter=0;
    localStatus.status=0;
    remoteStatus.status=0;
    newState.tryLock(0);
    newState.unlock();
}

void FlashWorker::slot_recvFrame(QByteArray data){
    remoteStatus.status=data.at(0);
    qDebug()<<"FirmwareWidget:remoteStatus="<<"unlock:"<<remoteStatus.attr.unlock<<" ok:"<<remoteStatus.attr.ok \
              <<" fatal:"<<remoteStatus.attr.fatal<<" counter:"<<remoteStatus.attr.counter;
    newState.tryLock(0);
    newState.unlock();
}

qint32 FlashWorker::ispGetStatus(){
    int requery=0;
    newState.tryLock(0);
    while(requery<MAX_REQUERY){
        if(requery++>0){
            QByteArray _getStatus;
            _getStatus.append(char(GET_STATUS));
            newState.tryLock(0);
            emit signal_sendFrame(_getStatus);
        }
        if(true==newState.tryLock(1000)){   //successful get status
            return ISP_CMD_OK;
        }
    }
    return ISP_CMD_TO;
}

qint32 FlashWorker::ispCmd(uint8_t cmd, QByteArray &data){
    if(cmd==char(GET_STATUS))return ispGetStatus();
    int resend=0;
    QByteArray frame;
    frame.append(cmd);
    frame.append(data);
    while(resend++<MAX_RETRY){
        emit signal_sendFrame(frame);
        if(ispGetStatus()==ISP_CMD_TO){
            return ISP_CMD_TO;
        }else{
            if(cmd==REBOOT){
                if(remoteStatus.attr.unlock==2 || remoteStatus.attr.counter!=0 || remoteStatus.attr.fatal!=0)return ISP_CMD_FATAL;
                return ISP_CMD_OK;
            }else if(remoteStatus.attr.fatal){
                return ISP_CMD_FATAL;
            }else if(remoteStatus.attr.counter!=localStatus.attr.counter){
                localStatus.status=remoteStatus.status;
                if(remoteStatus.attr.ok){
                    return ISP_CMD_OK;
                }else{
                    return ISP_CMD_NOK;
                }
            }
            //else continue
        }
    }
    return ISP_CMD_TO;
}
qint32 FlashWorker::ispCmdWithRetry(uint8_t cmd, QByteArray data,int fatalLevel){
    int ret;
    while(1){
        ret=ispCmd(cmd,data);
        if(ISP_CMD_OK == ret || ISP_CMD_NOK==ret){
            break;
        }else if(errorCounter>MAX_RETRY || ret==ISP_CMD_FATAL){
            emit flashStage(fatalLevel + ret);
            return ISP_CMD_FATAL;
        }
        errorCounter++;
    }
    errorCounter=0;
    return ret;
}
void FlashWorker::setFlashData(QByteArray data){
    flashData=data;
}

void FlashWorker::run(){
    if(flashData.size()==0 || flashData.size()%4!=0){
        emit flashStage(-1);
        return;
    }
    uint32_t head=((uint32_t*)flashData.data())[0];
    ((uint32_t*)flashData.data())[0]=0xFFFFFFFF;
    status_reset();
    do{
        emit flashStage(1);
        if(ispCmdWithRetry(REBOOT,QByteArray("\r\nM998\r\nreboot();\r\n"),-10)!=ISP_CMD_OK)break;
        emit flashStage(2);
        if(ispCmdWithRetry(SET_UNLOCK,QByteArray(1,char(1)),-20)!=ISP_CMD_OK)break;
        emit flashStage(3);
        if(ispCmdWithRetry(SET_SECTOR,QByteArray(1,char(dataSector)),-30)!=ISP_CMD_OK)break;
        emit flashStage(4);
        if(ispCmdWithRetry(SECTOR_ERASE,QByteArray(),-40)!=ISP_CMD_OK)break;
        
        //start write
        {
            QByteArray data;
            int ret;
            while(dataLp < flashData.length()){
                data=flashData.mid(dataLp,SECTOR_WRITE_BLOCK);
                emit flashStage(5);
                ret=ispCmdWithRetry(SECTOR_WRITE,data,-50);
                if(ISP_CMD_NOK == ret){
                    emit flashStage(3);
                    if(ispCmdWithRetry(SET_SECTOR,QByteArray(1,char(++dataSector)),-60)!=ISP_CMD_OK)break;
                    emit flashStage(4);
                    if(ispCmdWithRetry(SECTOR_ERASE,QByteArray(),-70)!=ISP_CMD_OK)break;
                    continue;
                }else if(ISP_CMD_OK != ret){
                    emit flashStage(-80 + ret);
                    return;
                }
                dataLp+=data.size();
                emit flashDataLp(dataLp);
            }
        }
        emit flashStage(6);
        if(ispCmdWithRetry(SET_SECTOR,QByteArray(1,char(4)),-90)!=ISP_CMD_OK)break;
        if(ispCmdWithRetry(SECTOR_WRITE,QByteArray((const char*)&head,4),-100)!=ISP_CMD_OK)break;
        emit flashStage(7);
        if(ispCmdWithRetry(SET_UNLOCK,QByteArray(1,char(0)),-110)!=ISP_CMD_OK)break;
        emit flashStage(0);
    }while(0);
    return;
    
}
