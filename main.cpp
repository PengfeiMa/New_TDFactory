#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QTextCodec>


MainWindow *mainWindow=0;
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        if(mainWindow){
            mainWindow->log(msg);
            if(!msg.endsWith("\n")){
                mainWindow->log(QString("\r\n"));
            }
        }
        fprintf(stderr, "%s\n", localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
        break;
    default:
        break;
    }
    fflush(stderr);
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    QApplication a(argc, argv);

    QString qpath = QDir::currentPath();

    QCoreApplication::setOrganizationName("微深科技");
    QCoreApplication::setOrganizationDomain("www.visentech.com");
    QCoreApplication::setApplicationName("GRAM");

    QSplashScreen splash(QPixmap(":/images/splash.png"));

    splash.setDisabled(true);//进制用户的输入事件相应

    splash.show();
    a.processEvents();

//    QElapsedTimer t;
//    t.start();
//    while(t.elapsed()<1000)
//    {
//        QCoreApplication::processEvents();
//    }

    QTranslator qtTranslator; 
    qtTranslator.load("zh_cn.qm"); 
    a.installTranslator(&qtTranslator); 
            
    MainWindow w;
    mainWindow=&w;
    w.setWindowTitle("GRAM Slicer 2.1 -- 微深科技 www.visentech.com");
//    w.setWindowTitle("GRAM 切片软件");
#ifdef INNER

#endif

    w.show();

    splash.finish(mainWindow);

    return a.exec();
}






