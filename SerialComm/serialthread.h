#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QObject>
#include <QThread>

class SerialPortWrapper;
class SerialThread : public QThread
{
    Q_OBJECT
public:
    friend class SerialPortWrapper;
    explicit SerialThread(SerialPortWrapper *serial,QObject *parent);
    ~SerialThread();
public:
signals:
    void opening(void);
    void closing(void);
private slots:
    bool open(void);
    bool close(void);
    void cleanup(void);
private:
    SerialPortWrapper *serial;
};

#endif // SERIALTHREAD_H
