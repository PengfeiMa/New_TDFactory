#pragma once

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QSettings>
#include <QIntValidator>

#include <SerialComm/seriallinklayer.h>

namespace Ui {
class serialWidget;
}

class QIntValidator;

class SerialWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialWidget(QWidget *parent = 0);
    SerialLinkLayer serial;
    ~SerialWidget();
    void show();
    void hide();
signals:
    void status(QString);
    void whenHide();
private slots:
    void showPortInfo(int idx);
    void on_applyButton_clicked();

private:
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();
private:
    Ui::serialWidget *ui;
    QSettings settings;
    QIntValidator *intValidator;
};





