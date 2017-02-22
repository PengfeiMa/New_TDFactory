#ifndef FLASHWORKER_H
#define FLASHWORKER_H

#include <QObject>
#include <QByteArray>
#include <stdint.h>
#include <QMutex>
#include <QThread>

namespace FirmwareProtocol{
    typedef union{
        struct _status_t{
            uint8_t unlock:2;
            uint8_t ok:1;
            uint8_t fatal:1;
            uint8_t counter:4;	//0~16
        }attr;
        uint8_t status;
    }status_t;
    typedef enum{
        GET_STATUS=0,
        SET_UNLOCK,	//unlock=1 -> stay in ISP,=0 -> run userapp
        SET_SECTOR,
        SECTOR_ERASE,
        SECTOR_WRITE,
        SECTOR_GET_CHECHSUM,
        ADDR_WRITE,
        REBOOT,
    }cmd_t;
}

class FlashWorker : public QThread
{
    Q_OBJECT
public:
    explicit FlashWorker(QObject *parent = 0);
    void setFlashData(QByteArray data);
    virtual void run();
signals:
    void signal_sendFrame(QByteArray);
    void flashStage(qint32 result);
    void flashDataLp(qint64 lp);
public slots:
    void slot_recvFrame(QByteArray data);

private:
    void status_reset();
    qint32 ispCmd(uint8_t cmd, QByteArray &data);
    qint32 ispCmdWithRetry(uint8_t cmd, QByteArray data,int fatalLevel);
    qint32 ispGetStatus();
    
    FirmwareProtocol::status_t localStatus,remoteStatus;
    qint64 dataLp,dataSector;
    int errorCounter;
    QMutex newState;
    QByteArray flashData;
};

#endif // FLASHWORKER_H
