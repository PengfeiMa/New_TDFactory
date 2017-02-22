#include "modelmodifydialog.h"
#include "ui_modelmodifydialog.h"

ModelModifyDialog::ModelModifyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelModifyDialog)
{
    ui->setupUi(this);
    ok=false;
}

ModelModifyDialog::~ModelModifyDialog()
{
    delete ui;
}


void ModelModifyDialog::on_buttonBox_accepted()
{
    value=getValue();
    ok=true;
    this->hide();
}

void ModelModifyDialog::setValue(QVector3D v){
    ui->x->setValue(v[0]);
    ui->y->setValue(v[1]);
    ui->z->setValue(v[2]);
}
QVector3D ModelModifyDialog::getValue(){
    QVector3D v;
    v[0]=ui->x->value();
    v[1]=ui->y->value();
    v[2]=ui->z->value();
    return v;
}



void ModelModifyDialog::setUniform(bool enable){
    ui->uniform->setChecked(enable);
    if(enable){
        connect(ui->x,SIGNAL(valueChanged(double)),ui->y,SLOT(setValue(double)));
        connect(ui->x,SIGNAL(valueChanged(double)),ui->z,SLOT(setValue(double)));
        ui->y->setEnabled(false);
        ui->z->setEnabled(false);
    }else{
        disconnect(ui->x,SIGNAL(valueChanged(double)),ui->y,SLOT(setValue(double)));
        disconnect(ui->x,SIGNAL(valueChanged(double)),ui->z,SLOT(setValue(double)));
        ui->y->setEnabled(true);
        ui->z->setEnabled(true);
    }
}


void ModelModifyDialog::on_uniform_toggled(bool checked)
{
    setUniform(checked);
}
