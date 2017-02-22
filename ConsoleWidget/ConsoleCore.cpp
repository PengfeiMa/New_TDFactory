#include "consolecore.h"
#include <QScrollBar>

ConsoleCore::ConsoleCore(QWidget *parent)
    : QPlainTextEdit(parent)
    , localEchoEnabled(false)
{
    document()->setMaximumBlockCount(10000);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);

}

void ConsoleCore::printData(const QByteArray &data)
{
    if(data[0]==0x08){
        QKeyEvent a(QEvent::KeyPress,Qt::Key_Backspace,0);
        QPlainTextEdit::keyPressEvent(&a);
    }else{
        insertPlainText(QString(data));
        QScrollBar *bar = verticalScrollBar();
        bar->setValue(bar->maximum());
    }
}
void ConsoleCore::printData(const QString data)
{
    insertPlainText(data);
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void ConsoleCore::operator << (const QString in){
    insertPlainText(in);
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}
void ConsoleCore::clearConsole(void){
    clear();
}

void ConsoleCore::setLocalEchoEnabled(bool set)
{
    localEchoEnabled = set;
}

void ConsoleCore::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        // skip processing
        break;
    default:
        if (localEchoEnabled)
            QPlainTextEdit::keyPressEvent(e);
        emit dataEntered(e->text().toLocal8Bit());
    }
}

void ConsoleCore::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void ConsoleCore::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void ConsoleCore::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}
