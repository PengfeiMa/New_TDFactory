#ifndef ConsoleWidget_H
#define ConsoleWidget_H

#include <QWidget>

namespace Ui {
class consoleWidget;
}

class ConsoleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConsoleWidget(QWidget *parent = 0,qint32 showEditbox=0);
    ~ConsoleWidget();
    void operator <<(const QString &in);
signals:
    void dataEntered(QByteArray data);

public slots:
    void printData(QByteArray data);
    void printData(QString data);
    void clearConsole(void);
private slots:
    void on_submit_clicked();

private:
    Ui::consoleWidget *ui;
};

#endif // ConsoleWidget_H
