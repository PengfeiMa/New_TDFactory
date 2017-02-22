#ifndef CONSOLECORE_H
#define CONSOLECORE_H

#include <QPlainTextEdit>

class ConsoleCore : public QPlainTextEdit
{
    Q_OBJECT

signals:
    void dataEntered(QByteArray data);

public:
    explicit ConsoleCore(QWidget *parent = 0);
    void operator <<(const QString in);
    void setLocalEchoEnabled(bool set);

public slots:
    void printData(const QByteArray &data);
    void printData(const QString data);
    void clearConsole(void);
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);

private:
    bool localEchoEnabled;

};

#endif // CONSOLECORE_H
