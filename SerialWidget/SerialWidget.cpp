#include "SerialWidget.h"
#include "ui_SerialWidget.h"

#include <QtSerialPort/QSerialPortInfo>

#include <QLineEdit>
#include <QDebug>

SerialWidget::SerialWidget(QWidget *parent) :
    QWidget(parent),
    serial(parent),
    ui(new Ui::serialWidget)
{
    ui->setupUi(this);
    intValidator = new QIntValidator(0, 4000000, this);

    ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(showPortInfo(int)));

    ui->baudRateBox->lineEdit()->setValidator(intValidator);
    fillPortsParameters();
    fillPortsInfo();

    updateSettings();
}

void SerialWidget::show(){
    fillPortsInfo();
    QWidget::show();
}
void SerialWidget::hide(){
    if(serial.isOpen()){
        emit status(QString("Serial Port")+ ui->serialPortInfoListBox->currentText() +QString("Opened"));
    }else{
        emit status(QString("Serial Port open failed"));
    }
    emit whenHide();
    QWidget::hide();
}

SerialWidget::~SerialWidget()
{
    delete ui;
}

void SerialWidget::showPortInfo(int idx)
{
    if (idx != -1) {
        QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
        ui->descriptionLabel->setText(tr("Description: %1").arg(list.at(1)));
        ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.at(2)));
        ui->locationLabel->setText(tr("Location: %1").arg(list.at(3)));
        ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.at(4)));
        ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.at(5)));
    }
}


void SerialWidget::fillPortsParameters()
{
    // fill baud rate (is not the entire list of available values,
    // desired values??, add your independently)
    ui->baudRateBox->addItem(QLatin1String("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QLatin1String("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QLatin1String("115200"), QSerialPort::Baud115200);

    QVariant v=settings.value("serial/baudRate");
    if(v!=QVariant()){
        ui->baudRateBox->lineEdit()->setText(v.toString());
    }else{
        ui->baudRateBox->setCurrentIndex(3);
    }

    // fill data bits
    ui->dataBitsBox->addItem(QLatin1String("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QLatin1String("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QLatin1String("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QLatin1String("8"), QSerialPort::Data8);

    ui->dataBitsBox->setCurrentIndex(3);

    // fill parity
    ui->parityBox->addItem(QLatin1String("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(QLatin1String("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(QLatin1String("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(QLatin1String("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(QLatin1String("Space"), QSerialPort::SpaceParity);

    ui->parityBox->setCurrentIndex(0);

    // fill stop bits
    ui->stopBitsBox->addItem(QLatin1String("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(QLatin1String("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QLatin1String("2"), QSerialPort::TwoStop);

    ui->stopBitsBox->setCurrentIndex(0);

    // fill flow control
    ui->flowControlBox->addItem(QLatin1String("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(QLatin1String("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(QLatin1String("XON/XOFF"), QSerialPort::SoftwareControl);

    ui->flowControlBox->setCurrentIndex(0);

}

void SerialWidget::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        list << info.portName()
             << info.description()
             << info.manufacturer()
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString())
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : QString());
        ui->serialPortInfoListBox->addItem(list.first(), list);
    }
    QVariant v=settings.value("serial/lastCom");
    if(v!=QVariant()){
        int index=ui->serialPortInfoListBox->findText(v.toString());
        if(index!=-1){
            ui->serialPortInfoListBox->setCurrentIndex(index);
            ui->status->setText(QString("load last used ")+v.toString());
            return;
        }
    }
    ui->status->setText(QString("load last used "));
}

void SerialWidget::updateSettings()
{
    settings.setValue("serial/lastCom",ui->serialPortInfoListBox->currentText());
    settings.setValue("serial/baudRate",ui->baudRateBox->currentText());
}


void SerialWidget::on_applyButton_clicked()
{
    if(serial.isOpen()){
        serial.close();
    }
    serial.setPortName(ui->serialPortInfoListBox->currentText());
    serial.setBaudRate(ui->baudRateBox->lineEdit()->text().toInt());
    //TODO:add more param
    if(serial.open(QIODevice::ReadWrite)==true){
        serial.setFlowControl(QSerialPort::NoFlowControl);
        serial.setRequestToSend(false);
        updateSettings();
        hide();
    }else{
        ui->status->setText(QString("Serial Port open failed"));
    }
}
