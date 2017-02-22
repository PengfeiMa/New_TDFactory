#include "mainwindow.h"


void MainWindow::on_actionSlice_triggered()
{
    //if(ui->triWidget->triangles.getNum()==0)return;
    SlicerDialog slicerDialog(this);
    slicerDialog.exec();
    if(!slicerDialog.ok) return;
    if(ui->triWidget->triangles.getNum()==0)
    {
        QMessageBox::warning(this,tr("请加载模型文件"),tr("尚未打开模型文件，不能启动切片引擎！"));
        return;
    }

    QString fileName=openFile.fileName();
    fileName=fileName.split('/').last();
    fileName=fileName.split('.').first();
    QSettings setting;
    if(setting.value("lastSavedFileName").isNull()==false){
        QString lastFileDir=setting.value("lastSavedFileName").toString();
        qDebug()<<"load setting"<<lastFileDir;
        lastFileDir=lastFileDir.left(lastFileDir.lastIndexOf('/')+1);
        fileName.insert(0,lastFileDir);
    }else{
        qDebug()<<"load setting failed";
    }
    QString outputPath = QFileDialog::getSaveFileName(this,tr("保存-GCode-路径文件"),fileName, tr("GCode RoutePath Files (*.gcode)"));
    if(outputPath.isEmpty()==false){
        setting.setValue("lastSavedFileName",outputPath);
        qDebug()<<"save setting"<<outputPath;
        m_fileName = openFile.fileName();
        openFile.setFileName("./model/temp.stl");
        ui->triWidget->triangles.saveAsStl(openFile);
        QString program = "CuraEngine_GRAM.exe";
        QStringList arguments;
        arguments<<"-v";
        arguments<<"-c"<<"./profile/"+slicerDialog.profileName;
        qDebug()<<ui->triWidget->triangles.triOffset;
        qDebug()<<ui->triWidget->XMAX<<ui->triWidget->YMAX<<ui->triWidget->ZMAX;
        qDebug()<<ui->triWidget->triangles.triOffset[0];
        qDebug()<<ui->triWidget->triangles.triOffset[1];
        qDebug()<<-ui->triWidget->triangles.triOffset[2];
        arguments<<"-s"<<QString("posx=%1").arg(ui->triWidget->XMAX*1000 + ui->triWidget->triangles.triOffset[0]*1000);
        arguments<<"-s"<<QString("posy=%1").arg(ui->triWidget->YMAX*1000 + ui->triWidget->triangles.triOffset[1]*1000);
        arguments<<"-s"<<QString("objectSink=%1").arg(-ui->triWidget->triangles.triOffset[2]*1000);
        
        if(slicerDialog.enableRaft==false){
            arguments<<"-s"<<"enableRaft=0";
        }else{
            arguments<<"-s"<<"enableRaft=1";
        }
        if(slicerDialog.enableSupport==false){
            arguments<<"-s"<<"enableSupport=0";
        }else{
            arguments<<"-s"<<"enableSupport=1";
        }

        arguments<<"-s"<<QString("temperature=%1").arg(slicerDialog.temperature);
        arguments<<"-s"<<QString("layerThickness=%1").arg(slicerDialog.layerThickness);
        arguments<<"-s"<<QString("infillLineSpacing=%1").arg(slicerDialog.infillLineSpacing);

        arguments<<"-o"<<outputPath;
        //stl file
        arguments<<openFile.fileName();
        qDebug()<<"exec "<<program<<arguments;
        slicer.start(program, arguments);
        //ui->dockWidget->show();
        progress->setProgress(0);
        connect(progress,SIGNAL(userCanceled()),&slicer,SLOT(kill()));
        progress->show();      
        }
}

void MainWindow::onSlicerStdoutReady(){
    QByteArray inf=slicer.readAll();
    if(inf.at(0)=='!'){
        progress->setProgress(10*(inf.at(1)-'0'));
        progress->setText(inf.mid(2,20).simplified());
    }else{
        progress->setText(inf.left(20).simplified());
    }

 // extract print time and filament weights
    QString subpri = "Print time:";
    QString subfil = "Filament:";
    int s_pos = inf.indexOf(subpri);
    int f_pos = 0;
    QString ptime,flen;
    if(s_pos > 0)
    {
        total_filaweight = 0.0;
        ptime = inf.mid(s_pos);
        f_pos = ptime.indexOf(subfil);
        flen = ptime.mid(f_pos);
        ptime.truncate(f_pos-2);
        ptime = ptime.mid(subpri.size() + 1);
        int rn_pos = flen.indexOf("\r\n");
        flen.truncate(rn_pos);
        flen = flen.mid(subfil.size()+ 1);
        total_filaweight = flen.toFloat()*3/1000;
        total_time = TimeHMSConvert(ptime);
     }
    qDebug("%s",inf.data());
}
QString MainWindow::TimeHMSConvert(QString s)
{
    int total_s = s.toInt();
    int nHour = total_s/3600;
    int nMin = (total_s - nHour*3600)/60;
    int nSec =  total_s - nHour*3600 - nMin*60;
    QString strTime = QString("%1h:%2m:%3s").arg(nHour, 2, 10, QChar('0')).arg(nMin, 2, 10, QChar('0')).arg(nSec, 2, 10, QChar('0'));
    return strTime;
}
void MainWindow::onSlicerFinished(int code,QProcess::ExitStatus status){
    progress->setText("Slice Successful...");
    progress->hide();

   //  showStatus(tr("估计打印耗时： ")+ total_time + " , 模型重量： " + QString("%1g").arg(total_filaweight));

    showResults(tr("估计打印耗时： ")+ total_time + " , 模型重量： " +
                          QString("%1g").arg(total_filaweight));

    openFile.setFileName(m_fileName);
    qDebug()<<"slicer Finished:"<<code<<status;
}
void MainWindow::onSlicerError(QProcess::ProcessError err){
    progress->setText("Slice Failed...");
    qDebug()<<"slicer Error:"<<err;
}



