#ifndef MODELMODIFYDIALOG_H
#define MODELMODIFYDIALOG_H

#include <QDialog>
#include <QVector3D>

namespace Ui {
class ModelModifyDialog;
}

class ModelModifyDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ModelModifyDialog(QWidget *parent = 0);
    ~ModelModifyDialog();
    QVector3D value;
    bool  ok;
    void setValue(QVector3D v);
    QVector3D getValue();
    void setUniform(bool enable);
private slots:
    void on_buttonBox_accepted();
    
    void on_uniform_toggled(bool checked);
    
private:
    Ui::ModelModifyDialog *ui;
};

#endif // MODELMODIFYDIALOG_H
