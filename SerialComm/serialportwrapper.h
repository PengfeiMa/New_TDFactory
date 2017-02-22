#ifndef SERIALPORTWRAPPER_H
#define SERIALPORTWRAPPER_H

#include <QSerialPort>
class SerialThread;
class SerialPortWrapper : public QSerialPort
{
    Q_OBJECT
public:
    explicit SerialPortWrapper(QObject *parent);
    friend class SerialThread;
    virtual bool open(OpenMode mode);
    virtual void close();
    QByteArray readAll();
    qint64 write(QByteArray &data);
    
private:
    volatile int updateFlag;
    SerialThread *thread;
private slots:  //used in SerialThread
    void safe_open(void);
    void safe_close(void);
};

#endif // SERIALPORTWRAPPER_H







