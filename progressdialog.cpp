#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::on_pushButton_clicked()
{
    emit userCanceled();
    this->hide();
}
void ProgressDialog::setText(QString msg){
    ui->label->setText(msg);
}
void ProgressDialog::setProgress(int progress){
    ui->progressBar->setValue(progress);
}
