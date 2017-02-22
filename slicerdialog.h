#ifndef SLICERDIALOG_H
#define SLICERDIALOG_H

#include <QDialog>
#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QSettings>

namespace Ui {
class SlicerDialog;
}

class SlicerDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SlicerDialog(QWidget *parent = 0);
    ~SlicerDialog();
    
    
public:
    void refreshList();
private slots:
    void on_btnAdd_clicked();
    
    void on_btnEdit_clicked();
    
    void on_btnDel_clicked();
    
    void on_btnOk_accepted();

    
public:
    QString profileName;
    int  temperature;
    int  infillLineSpacing;// val_3 填充线宽
    int  layerThickness;  // val_2 层高   50-300
    int  infillLinewidth;
    bool enableRaft;
    bool enableSupport;
    bool ok;
private:
    Ui::SlicerDialog *ui;
    QSettings settings;
};

#endif // SLICERDIALOG_H
