#include "ConsoleWidget.h"
#include "ui_ConsoleWidget.h"

ConsoleWidget::ConsoleWidget(QWidget *parent,qint32 showEditbox) :
    QWidget(parent),
    ui(new Ui::consoleWidget)
{
    ui->setupUi(this);
    if(showEditbox==0){
        ui->editbox->setHidden(true);
        ui->submit->setHidden(true);
    }
    connect(ui->console,SIGNAL(dataEntered(QByteArray)),this,SIGNAL(dataEntered(QByteArray)));
}

void ConsoleWidget::printData(QByteArray data)
{
    int size=data.length();
    QString hex;
    for(int i=0;i<size;i++){
        hex.sprintf("%02X ",(quint8)data.at(i));
        ui->console->printData(hex);
    }
}
void ConsoleWidget::printData(QString data)
{
    ui->console->printData(data);
}
void ConsoleWidget::clearConsole(void){
    ui->console->clearConsole();
}

void ConsoleWidget::operator << (const QString &in){
   (*(ui->console))<<in;
}
ConsoleWidget::~ConsoleWidget()
{
    delete ui;
}

void ConsoleWidget::on_submit_clicked()
{
    emit dataEntered(ui->editbox->text().append('\n').toLocal8Bit());
}
