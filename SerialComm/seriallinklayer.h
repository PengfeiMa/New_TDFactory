#pragma once

#include "serialportwrapper.h"
#include <stdint.h>



class SerialLinkLayer //: public SerialPortWrapper
         : public QSerialPort
{
    Q_OBJECT
public:
    explicit SerialLinkLayer(QObject *parent = 0);
signals:
    void frameReceived(QByteArray data);
public slots:
    void sendFrame(QByteArray data);

private slots:
    void readFromSerial(void);
private:
    void rxStateMachine(QByteArray &data);
    int checkHeader(uint8_t data);
    int streamInput(uint8_t data);
private:
    uint8_t state,checksum,frameSize;
    QByteArray rxBuffer;
};

