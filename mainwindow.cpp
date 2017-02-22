#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    total_filaweight(0.0)
{
    ui->setupUi(this);
    serialWidget = new SerialWidget(this);
    serialWidget->setWindowFlags(Qt::Dialog);
    serialWidget->setWindowModality(Qt::WindowModal);
    firmwareWidget = new FirmwareWidget(this);
    firmwareWidget->setWindowFlags(Qt::Dialog);
    firmwareWidget->setWindowModality(Qt::WindowModal);
    ui->console->hide();
    progress = new ProgressDialog(this);
    progress->setModal(true);
    fileName = "";
//    modelnum=0;

    permanent = new QLabel(this);
    permanent->setFrameStyle(QFrame::Box|QFrame::Sunken);
    ui->status->addPermanentWidget(permanent);


#ifdef MINMENU
    ui->actionConn->setEnabled(false);
    ui->actionDisconnect->setEnabled(false);
    ui->actionFirmware->setEnabled(false);
    ui->actionConsole->setEnabled(false);
    ui->actionClear_Console_2->setEnabled(false);
#endif


    ui->dockWidget->setHidden(true);
    connect(firmwareWidget,SIGNAL(signal_sendFrame(QByteArray)),&this->serialWidget->serial,SLOT(sendFrame(QByteArray)));
    connect(&serialWidget->serial,SIGNAL(frameReceived(QByteArray)),firmwareWidget,SLOT(slot_recvFrame(QByteArray)));
    
    slicer.setReadChannel(QProcess::StandardError);
    connect(&slicer,SIGNAL(readyRead()),this,SLOT(onSlicerStdoutReady()));
    connect(&slicer,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(onSlicerFinished(int,QProcess::ExitStatus)));
    connect(&slicer,SIGNAL(error(QProcess::ProcessError)),this,SLOT(onSlicerError(QProcess::ProcessError)));
    
    //connect(ui->serialWidget,SIGNAL(whenHide()),this,SLOT(applyFileStatus()));
    connect(serialWidget,SIGNAL(status(QString)),this,SLOT(showStatus(QString)));
    updateFileMenu();
    updateModelMenu();
    readSettings();
}



void MainWindow::updateFileMenu(){
    if(ui->triWidget->triangles.getNum()){
        ui->actionSave->setEnabled(true);
        ui->actionSaveAs->setEnabled(true);
        ui->actionClose->setEnabled(true);
        this->setWindowTitle(openFile.fileName().split('/').last());
    }else{
        ui->actionSave->setEnabled(false);
        ui->actionSaveAs->setEnabled(false);
        ui->actionClose->setEnabled(false);
        this->setWindowTitle("GRAM");
    }
}

void MainWindow::updateModelMenu(){
    ui->actionTranslate->setChecked(false);
    ui->actionView->setChecked(false);
    ui->actionScale->setChecked(false);
    switch(ui->triWidget->mode){
    case 1:
        ui->actionTranslate->setChecked(true);
        break;
    case 2:
        ui->actionScale->setChecked(true);
        break;
    default:
        ui->actionView->setChecked(true);
        break;
    }
}


void MainWindow::serial2console(QByteArray data){
    ui->console->printData(QString(data));
}
void MainWindow::showStatus(QString state){

    ui->status->showMessage(state,0);

}
void MainWindow::showResults(QString state)
{
    permanent->setText(state);
    //ui->status->addWidget(permanent);
    ui->status->update();
 }
void MainWindow::log(QString data){
    ui->log->printData(data);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int  MainWindow::STL_SIZE(double& XSize, double& YSize, double& ZSize)
{

    int num=ui->triWidget->STL_SIZE(XSize,YSize,ZSize);


    if(num == 0)
        return 0;

//    Mod_Xsize=XSize;
//    Mod_Ysize=YSize;
//    Mod_Zsize=ZSize;

    if( 2 * ui->triWidget->XMAX < XSize)
    {
        double xx = 2 * ui->triWidget->XMAX;
        QMessageBox::information(this,tr("提示"),
                                 tr("模型X方向限额 ")+ QString("%1 mm  ").arg(xx) +
                                 tr("，请减少模型X方向尺寸"));

    }
    if( 2 * ui->triWidget->YMAX < YSize)
    {
        double yy = 2 * ui->triWidget->YMAX;
        QMessageBox::information(this,tr("提示"),
                                 tr("模型Y方向限额 ")+ QString("%1 mm  ").arg(yy) +
                                 tr("，请减少模型Y方向尺寸"));

    }
    if( ui->triWidget->ZMAX < ZSize)
    {
        double zz = ui->triWidget->ZMAX;
        QMessageBox::information(this,tr("提示"),
                                 tr("模型Z方向限额 ")+ QString("%1 mm  ").arg(zz) +
                                 tr("，请减少模型Z方向尺寸"));

    }



    return 1;

}
//int MainWindow::ModelNum(int i)
//{
//    return i;
//}
void MainWindow::on_actionOpen_triggered()
{

    fileName = QFileDialog::getOpenFileName(this,tr("Open STL Model"),QDir::currentPath(), tr("STL Model Files (*.stl)"));
    if(fileName.isEmpty()==false){
        openFile.setFileName(fileName);
        double XSize,YSize,ZSize;
        if(openFile.open(QIODevice::ReadOnly)){
            ui->status->showMessage(fileName + tr(" open successful"));
            ui->triWidget->triangles.stream.setDevice(&openFile);
            int num=ui->triWidget->triangles.loadStreamAsStlFile();


            STL_SIZE(XSize,YSize,ZSize);

//            ui->status->showMessage(tr("模型大小:  ")+ "X方向 "+QString("%1 mm  ,").arg(XSize)
//                                                  + "Y方向 "+QString("%1 mm  ,").arg(YSize)
//                                                  + "Z方向 "+QString("%1 mm ").arg(ZSize)
//                                    );





            showResults(tr("模型大小:  ")+ "X方向 "+QString("%1 mm  ,").arg(XSize)
                               + "Y方向 "+QString("%1 mm  ,").arg(YSize)
                               + "Z方向 "+QString("%1 mm ").arg(ZSize));



            qDebug("MainWindow:read stl num=%d",num);
            openFile.close();
        }

        qDebug("Xsize %.2f mm",XSize);
        qDebug("Ysize %.2f mm",YSize);
        qDebug("Zsize %.2f mm",ZSize);

    }
    updateFileMenu();
//    ModelNum(modelnum++);
   //
    ui->triWidget->updateGL();
    qDebug()<<"MainWindow::on_actionOpen_triggered()";
}

void MainWindow::on_actionSave_triggered()
{
    ui->triWidget->triangles.saveAsStl(openFile);
}

void MainWindow::on_actionSaveAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save STL Model"),QDir::currentPath(), tr("STL Model Files (*.stl)"));
    if(fileName.isEmpty()==false){
        openFile.setFileName(fileName);
        ui->triWidget->triangles.saveAsStl(openFile);
    }
    updateFileMenu();
}

void MainWindow::on_actionClose_triggered()
{
    ui->triWidget->triangles.clear();
    updateFileMenu();
    ui->triWidget->updateGL();
}


void MainWindow::on_actionConn_triggered()
{
    serialWidget->show();
}
void MainWindow::on_actionDisconnect_triggered()
{
    serialWidget->serial.close();
    showStatus(tr("Serialport close successful"));
}
void MainWindow::on_actionClear_Console_triggered()
{
    ui->console->clearConsole();
    ui->log->clearConsole();
}


void MainWindow::on_actionFirmware_triggered()
{
    firmwareWidget->show();
}


void MainWindow::on_actionView_triggered()
{
    ui->triWidget->mode=0;
    updateModelMenu();
}

void MainWindow::on_actionTranslate_triggered()
{
    ui->triWidget->mode=1;
    updateModelMenu();
}

void MainWindow::on_actionScale_triggered()
{
    ui->triWidget->mode=2;
    updateModelMenu();
}

void MainWindow::on_actionRotateManual_triggered()
{
    ModelModifyDialog input(this);
    //input.setWindowModality(Qt::ApplicationModal);
    input.setWindowTitle(tr("--调整模型旋转参数--"));
    input.exec();
    if(input.ok){
        QVector3D r;
        r[0]=input.value[0];
        r[1]=input.value[1];
        r[2]=input.value[2];
        ui->triWidget->triangles.rotate(r);
        double XSize,YSize,ZSize;
        STL_SIZE(XSize,YSize,ZSize);
        ui->status->showMessage(tr("模型大小:  ")+ "X方向 "+QString("%1 mm  ,").arg(XSize)
                                              + "Y方向 "+QString("%1 mm  ,").arg(YSize)
                                              + "Z方向 "+QString("%1 mm ").arg(ZSize)
                                );
    }
}

void MainWindow::on_actionScaleManual_triggered()
{
    ModelModifyDialog input(this);
    //input.setWindowModality(Qt::ApplicationModal);
    input.setWindowTitle(tr("--调整模型缩放参数--"));
    input.setUniform(true);
    input.setValue(QVector3D(1.0f,1.0f,1.0f));
    input.exec();
    if(input.ok){
        QVector3D r;
        r[0]=input.value[0];
        r[1]=input.value[1];
        r[2]=input.value[2];
        ui->triWidget->triangles.scale(r);
        double XSize,YSize,ZSize;
        STL_SIZE(XSize,YSize,ZSize);
        ui->status->showMessage(tr("模型大小:  ")+ "X方向 "+QString("%1 mm  ,").arg(XSize)
                                              + "Y方向 "+QString("%1 mm  ,").arg(YSize)
                                              + "Z方向 "+QString("%1 mm ").arg(ZSize)
                                );
    }
}

void MainWindow::on_actionTranslateManual_triggered()
{
    ModelModifyDialog input(this);
    //input.setWindowModality(Qt::ApplicationModal);
    input.setWindowTitle(tr("--调整模型平移参数--"));
    input.exec();
    if(input.ok){
        QVector3D r;
        r[0]=input.value[0];
        r[1]=input.value[1];
        r[2]=input.value[2];
        ui->triWidget->triangles.translate(r);
        double XSize,YSize,ZSize;
        STL_SIZE(XSize,YSize,ZSize);
        ui->status->showMessage(tr("模型大小:  ")+ "X方向 "+QString("%1 mm  ,").arg(XSize)
                                              + "Y方向 "+QString("%1 mm  ,").arg(YSize)
                                              + "Z方向 "+QString("%1 mm ").arg(ZSize)
                                );
    }
}

void MainWindow::on_actionPutOnPlatform_triggered()
{
    ui->triWidget->triangles.triOffset[2]=0;
    ui->triWidget->updateGL();
}

void MainWindow::on_actionCenterOnPlatform_triggered()
{
    ui->triWidget->triangles.triOffset[0]=0;
    ui->triWidget->triangles.triOffset[1]=0;
    ui->triWidget->updateGL();
}


void MainWindow::on_actionAbout_triggered()
{
//    QMessageBox about;
//    about.setText("GRAM version 0.1\r\n3dprinter slicer\r\n");
//    about.exec();

     QDir dir("./");

     QString qspath = dir.absolutePath();
     QString filename = "/GRAM manual.pdf";
     qspath.append(filename);


    const wchar_t * filepath = reinterpret_cast<const wchar_t *>(qspath.utf16());

    ShellExecute(NULL, L"open",L"IEXPLORE.EXE",filepath,L" ", SW_SHOW);
}

void MainWindow::on_actionConsole_triggered()
{
    ui->dockWidget->show();
}

void MainWindow::on_actionClear_Console_2_triggered()
{
    ui->log->clearConsole();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}
void MainWindow::writeSettings()
{
       QSettings settings;
       settings.beginGroup("MainWindow");
       settings.setValue("size", size());
       settings.setValue("pos", pos());
       settings.endGroup();
}

void MainWindow::readSettings()
{
       QSettings settings;
       settings.beginGroup("MainWindow");
       resize(settings.value("size", QSize(800, 800)).toSize());
       move(settings.value("pos", QPoint(200, 200)).toPoint());
       settings.endGroup();
}
void MainWindow::closeEvent(QCloseEvent*event)
{
    if (maybeSave()||(ui->triWidget->triangles.getNum() == 0)) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}
bool MainWindow::maybeSave()
{
    bool ret = false;
    QMessageBox box;
    if( ui->triWidget->triangles.getNum() > 0 )
    {
        box.setWindowTitle(tr("警告"));
        box.setIcon(QMessageBox::Warning);
        box.setText(openFile.fileName() + tr("模型尚未保存，是否保存？"));
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        if(box.exec() == QMessageBox::Yes) //如果选择保存文件，则执行保存操作
        {
            on_actionSaveAs_triggered();
            //ui->triWidget->triangles.saveAsStl(openFile);
            ret = true;
        }
    }
    ret = true;
    return ret;    
}


void MainWindow::on_action_restore_triggered()
{

      if(fileName.isEmpty()==false){
        openFile.setFileName(fileName);
        double XSize,YSize,ZSize;
        if(openFile.open(QIODevice::ReadOnly)){
            ui->status->showMessage(fileName + tr(" open successful"));
            ui->triWidget->triangles.stream.setDevice(&openFile);
            int num=ui->triWidget->triangles.loadStreamAsStlFile();


            STL_SIZE(XSize,YSize,ZSize);
            ui->status->showMessage(tr("模型大小:  ")+ "X方向 "+QString("%1 mm  ,").arg(XSize)
                                                  + "Y方向 "+QString("%1 mm  ,").arg(YSize)
                                                  + "Z方向 "+QString("%1 mm ").arg(ZSize)
                                    );

            qDebug("MainWindow:read stl num=%d",num);
            openFile.close();
        }

        qDebug("Xsize %.2f mm",XSize);
        qDebug("Ysize %.2f mm",YSize);
        qDebug("Zsize %.2f mm",ZSize);

    }
    updateFileMenu();
   //
    ui->triWidget->updateGL();

}

void MainWindow::on_actionType_triggered()
{
    TypeDialog input(this);
    connect(&input,SIGNAL(closeapp()),this,SLOT(on_actionExit_triggered()));
    input.setWindowTitle(tr("--调整机器型号--"));
    input.exec();

}
