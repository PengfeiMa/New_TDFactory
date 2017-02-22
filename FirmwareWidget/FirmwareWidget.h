#pragma once

#include <QWidget>
#include <QSettings>
#include <SerialComm/serialportwrapper.h>
#include <QByteArray>
#include <stdint.h>
#include <QFile>
#include "flashWorker.h"

namespace Ui {
class firmwareWidget;
}

class FirmwareWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FirmwareWidget(QWidget *parent = 0);
    ~FirmwareWidget();

signals:
    void signal_sendFrame(QByteArray);
    
public slots:
    void slot_recvFrame(QByteArray data);
    void whenFlashStaged(qint32 result);
    void whenFlashDataLp(qint64 dataLp);
private slots:
    void on_btnOpen_clicked();
    void on_btnFlash_clicked();
    
private:
    Ui::firmwareWidget *ui;
    
    QFile fp;
    QByteArray flashData;
    FlashWorker *worker;
};





