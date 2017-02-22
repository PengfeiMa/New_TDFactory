#include "slicerdialog.h"
#include "ui_slicerdialog.h"
#include <QDesktopServices>
#include <QDir>
#include <QUrl>
#include <QDebug>


SlicerDialog::SlicerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SlicerDialog)
{
    ui->setupUi(this);
    refreshList();
    this->ok=false;
    infillLinewidth = 400;

    ui->btnOk->button(QDialogButtonBox::Ok)->setText("导出Gcode");
    ui->btnOk->button(QDialogButtonBox::Cancel)->setText("关闭");

}

SlicerDialog::~SlicerDialog()
{
    delete ui;
}

void SlicerDialog::on_btnAdd_clicked()
{
    bool ok=false;
    QString profileName = QInputDialog::getText(this, tr("New Profile Name"),tr("Name"),QLineEdit::Normal,"New "+ui->cmbProfile->currentText(),&ok);      
    if (ok && !profileName.isEmpty() && !ui->cmbProfile->currentText().isEmpty() ){
        if(profileName==ui->cmbProfile->currentText())return;
        if(!ui->cmbProfile->currentText().compare("High Quality"))return;
        if(!ui->cmbProfile->currentText().compare("Normal  Quality"))return;
        if(!ui->cmbProfile->currentText().compare("Low Quality"))return;
        QFile::copy("./profile/"+ui->cmbProfile->currentText(),"./profile/"+profileName);  
        refreshList();
        ui->cmbProfile->setCurrentIndex(ui->cmbProfile->findText(profileName));
    }
}

void SlicerDialog::refreshList(){
    ui->cmbProfile->clear();
    QDir dir("./");
    if(!dir.exists("./profile")){
        dir.mkdir("./profile");
    }
    dir.cd("profile");
    QStringList profileList=dir.entryList(QDir::Files);
    ui->cmbProfile->addItems(profileList);
    QVariant v=settings.value("slicerdialog/lastProfile");
    int i=ui->cmbProfile->findText(v.toString());
    if(i>=0){
        ui->cmbProfile->setCurrentIndex(i);
    }

    {//other slicer param
       if(settings.contains("slicerdialog/lastTemperature")){
           ui->tempVal->setValue(settings.value("slicerdialog/lastTemperature").toInt());  
       }
       if(settings.contains("slicerdialog/lastInfillLineSpacing")){
           int temp = settings.value("slicerdialog/lastInfillLineSpacing").toInt();
        //   temp = temp/100;
           ui->tempVal_3->setValue(temp);
       }
       if(settings.contains("slicerdialog/lastLayerThickness")){
           ui->tempVal_2->setValue(settings.value("slicerdialog/lastLayerThickness").toInt());
       }
       ui->enableRaft->setChecked(settings.value("slicerdialog/lastRaft").toBool());
       ui->enableSupport->setChecked(settings.value("slicerdialog/lastSupport").toBool());
    }
    //qDebug()<<"SlicerDialog::refreshList v="<<v<<"index="<<i;
}

void SlicerDialog::on_btnEdit_clicked()
{
    /*if(!ui->cmbProfile->currentText().compare("High Quality"))return;
    if(!ui->cmbProfile->currentText().compare("Normal  Quality"))return;
    if(!ui->cmbProfile->currentText().compare("Low Quality"))return;*/
    if ( !ui->cmbProfile->currentText().isEmpty() ){
        QDesktopServices::openUrl(QUrl::fromLocalFile( QDir::currentPath() + "/profile/" + ui->cmbProfile->currentText()));
    }
}

void SlicerDialog::on_btnDel_clicked()
{
    if ( !ui->cmbProfile->currentText().isEmpty() ){
        if(!ui->cmbProfile->currentText().compare("High Quality"))return;
        if(!ui->cmbProfile->currentText().compare("Normal  Quality"))return;
        if(!ui->cmbProfile->currentText().compare("Low Quality"))return;
        QDir::current().remove("./profile/"+ui->cmbProfile->currentText());
        refreshList();
    }
}

void SlicerDialog::on_btnOk_accepted()
{
    this->temperature=ui->tempVal->value();
    this->infillLineSpacing = ui->tempVal_3->value();
    this->layerThickness = ui->tempVal_2->value();
    this->enableRaft=ui->enableRaft->isChecked();
    this->enableSupport=ui->enableSupport->isChecked();
    this->profileName=ui->cmbProfile->currentText();
    if(!profileName.isEmpty()){
        this->ok=true;
        settings.setValue("slicerdialog/lastProfile",profileName);
        settings.setValue("slicerdialog/lastRaft",enableRaft);
        settings.setValue("slicerdialog/lastSupport",enableSupport);
        settings.setValue("slicerdialog/lastTemperature",temperature);
        settings.setValue("slicerdialog/lastInfillLineSpacing",infillLineSpacing);
        settings.setValue("slicerdialog/lastLayerThickness",layerThickness);
        //qDebug()<<"SlicerDialog::on_btnOk_accepted profileName="<<profileName;
    }
}



