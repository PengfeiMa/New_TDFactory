#include "typedialog.h"
#include "ui_typedialog.h"
#include <QApplication>



TypeDialog::TypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TypeDialog)
{
    ui->setupUi(this);

    QSettings settings;
    settings.beginGroup("MainWindow");

    oldtype = settings.value("type").toInt();

    if(oldtype == 1)
    {
          ui->radioButtonG1->setChecked(true);
          ui->radioButtonG2->setChecked(false);
          //settings.setValue("type",1);
    }
    else if(oldtype == 2)
    {
        ui->radioButtonG2->setChecked(true);
        ui->radioButtonG1->setChecked(false);
       // settings.setValue("type",2);
    }
    else if(oldtype == 3)
    {
        ui->radioButtonOthers->setChecked(true);
        ui->radioButtonOthers->setChecked(false);
       // settings.setValue("type",3);
    }
    settings.endGroup();
}

TypeDialog::~TypeDialog()
{
    delete ui;
}

void TypeDialog::on_buttonBox_accepted()
{

    if(ui->radioButtonG1->isChecked() && (oldtype == 1))
    {
        return;
    }
    if(ui->radioButtonG2->isChecked() && (oldtype == 2))
    {
        return;
    }
    if(ui->radioButtonOthers->isChecked() && (oldtype == 3))
    {
        return;
    }

    QSettings settings;
    settings.beginGroup("MainWindow");

    if(ui->radioButtonG1->isChecked())
    {
       settings.setValue("type",1);
    }
    else if(ui->radioButtonG2->isChecked())
    {
       settings.setValue("type",2);
    }
    // default value type == 2
    else if(ui->radioButtonOthers->isChecked())
    {
       settings.setValue("type",3);

       settings.setValue("valueX",ui->spinBoxX->value());
       settings.setValue("valueY",ui->spinBoxY->value());
       settings.setValue("valueZ",ui->spinBoxZ->value());

    }
//    else settings.setValue("type",1);

    settings.endGroup();

    QMessageBox msgBox;
       msgBox.setText("机器型号选择已改，请重启切片软件生效！");
       msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
      int ret = msgBox.exec();

      switch (ret) {
        case QMessageBox::Ok:
           // qApp->closeAllWindows();
            emit closeapp();
            break;
         default:
            // should never be reached
            break;
      }

}

void TypeDialog::on_buttonBox_rejected()
{

}



void TypeDialog::on_radioButtonOthers_toggled(bool checked)
{
    if(checked)
    {
        ui->spinBoxY->setEnabled(true);
        ui->spinBoxZ->setEnabled(true);
        ui->spinBoxX->setEnabled(true);
    }
    else
    {
        ui->spinBoxY->setDisabled(true);
        ui->spinBoxZ->setDisabled(true);
        ui->spinBoxX->setDisabled(true);

    }

}
