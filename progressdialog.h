#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = 0);
    ~ProgressDialog();
    void setText(QString msg);
    void setProgress(int progress);
signals:
    void userCanceled(void);
private slots:
    void on_pushButton_clicked();

private:
    Ui::ProgressDialog *ui;
};

#endif // PROGRESSDIALOG_H
