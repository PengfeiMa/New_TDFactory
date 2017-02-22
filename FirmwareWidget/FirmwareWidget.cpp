#include "FirmwareWidget.h"
#include "ui_FirmwareWidget.h"
#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>
#include <QStringList>



FirmwareWidget::FirmwareWidget(QWidget *parent):
    QWidget(parent),
    ui(new Ui::firmwareWidget)
{
    ui->setupUi(this);
    worker=new FlashWorker(this);
    connect(worker,SIGNAL(signal_sendFrame(QByteArray)),SIGNAL(signal_sendFrame(QByteArray)));
    connect(worker,SIGNAL(flashStage(qint32)),this,SLOT(whenFlashStaged(qint32)));
    connect(worker,SIGNAL(flashDataLp(qint64)),this,SLOT(whenFlashDataLp(qint64)));
}
FirmwareWidget::~FirmwareWidget(){
    
}

void FirmwareWidget::slot_recvFrame(QByteArray data){
    worker->slot_recvFrame(data);
}

void FirmwareWidget::whenFlashStaged(qint32 result){
    qDebug()<<"FirmwareWidget::whenFlashFinshed return "<<result;
    switch(result){
    case 0:
        ui->btnOpen->setEnabled(true);
        ui->status->setText("Flash Successful");
        break;
    case 1:
        ui->status->setText("Reboot...");
        break;
    case 2:
        ui->status->setText("Unlock...");
        break;
    case 3:
        ui->status->setText("Sector...");
        break;
    case 4:
        ui->status->setText("Erase...");
        break;
    case 5:
        ui->status->setText("Writing...");
        break;
    case 6:
        ui->status->setText("Writing Head...");
        break;
    case 7:
        ui->status->setText("Lock...");
        break;
    case 8:
        ui->status->setText("Run...");
        break;
    default:
        ui->btnFlash->setEnabled(true);
        ui->btnOpen->setEnabled(true);
        ui->status->setText(QString("Flash Failed %1").arg(result));
        break;
    }
}

void FirmwareWidget::whenFlashDataLp(qint64 dataLp){
    ui->progess->setValue(dataLp);
}

void FirmwareWidget::on_btnOpen_clicked()
{
    QStringList items;
    items<<"true"<<"false";
    QString text = QFileDialog::getOpenFileName(this,"Open Firmware",QDir::currentPath(),"Bin Files (*.bin)");
    if(text.isEmpty()==false){
        if(fp.isOpen())fp.close();
        fp.setFileName(text);
        if(fp.open(QIODevice::ReadOnly)){
            ui->edtFileName->setText(text);
        }
        flashData=fp.readAll();
        ui->progess->setMaximum(fp.size());
        ui->progess->setMinimum(0);
        ui->progess->setValue(0);
        ui->btnFlash->setEnabled(true);
    }
}

void FirmwareWidget::on_btnFlash_clicked()
{
    ui->btnFlash->setEnabled(false);
    ui->btnOpen->setEnabled(false);
    worker->setFlashData(flashData);
    worker->start();
}



