#ifndef TYPEDIALOG_H
#define TYPEDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QMessageBox>

namespace Ui {
class TypeDialog;
}

class TypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TypeDialog(QWidget *parent = 0);
    ~TypeDialog();
    bool ok;
    int oldtype;


signals:
    void closeapp();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_radioButtonOthers_toggled(bool checked);

private:
    Ui::TypeDialog *ui;
};

#endif // TYPEDIALOG_H
