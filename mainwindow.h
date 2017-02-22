#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include "SerialWidget/SerialWidget.h"
#include "FirmwareWidget/FirmwareWidget.h"
#include <QProcess>
#include <QFileDialog>
#include <QDir>
#include <QThread>
#include <QDebug>
#include "modelmodifydialog.h"
#include "progressdialog.h"
#include "slicerdialog.h"
#include "typedialog.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QSplashScreen>
#include <QElapsedTimer>
#include <QLabel>
#include "triglwidget.h"



//#define MINMENU
//#define OEM

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QFile openFile;
    void updateFileMenu();
    void updateModelMenu();
    void writeSettings();
    void readSettings();
    void closeEvent(QCloseEvent*event);
    QString TimeHMSConvert(QString s);
    bool maybeSave();    
    int  STL_SIZE(double & XSize, double & YSize, double & ZSize);
    QString fileName;
    QLabel* permanent; 
//    int ModelNum(int i);
//    int modelnum;


public slots:
    void showStatus(QString state);
    void showResults(QString state);
    void serial2console(QByteArray);
    void log(QString);


private slots:
    void on_actionOpen_triggered();
    void on_actionConn_triggered();
    
    void on_actionClear_Console_triggered();
    
    void on_actionFirmware_triggered();
    
    void on_actionSave_triggered();
    
    void on_actionSaveAs_triggered();
    
    void on_actionClose_triggered();
    
    void on_actionView_triggered();
    
    void on_actionTranslate_triggered();
    
    void on_actionScale_triggered();
    
    void on_actionRotateManual_triggered();
    
    void on_actionScaleManual_triggered();
    
    void on_actionTranslateManual_triggered();
    
    void on_actionPutOnPlatform_triggered();
    
    void on_actionCenterOnPlatform_triggered();
    
    void on_actionSlice_triggered();
    
private slots:
    void onSlicerStdoutReady();
    void onSlicerFinished(int code,QProcess::ExitStatus status);
    void onSlicerError(QProcess::ProcessError err);
    void on_actionDisconnect_triggered();
    
    void on_actionAbout_triggered();
    void on_actionConsole_triggered();

    void on_actionClear_Console_2_triggered();

    void on_actionExit_triggered();

    void on_action_restore_triggered();

    void on_actionType_triggered();

private:
    Ui::MainWindow *ui;
    SerialWidget *serialWidget;
    FirmwareWidget *firmwareWidget;
    QProcess slicer;
    ProgressDialog *progress;
    float total_filaweight;
    QString total_time;
    QString m_fileName;



};
#include <ui_mainwindow.h>

#endif // MAINWINDOW_H
