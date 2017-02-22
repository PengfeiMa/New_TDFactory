#include "stlcontainor.h"
#include <QDebug>
#include <QtOpenGL>
#include <stdint.h>
#include <QMessageBox>
#include <math.h>
#include <QQuaternion>

#define MAX_TRIANGLE 2147483647





StlContainor::StlContainor()
{
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
}

int StlContainor::loadStreamAsStlFile(){
    QByteArray head(80,0);
    stream.readRawData(head.data(),80);
    if(head.indexOf('\n')>=0){
        qDebug("loadStreamAsAscii");
        if(0==loadStreamAsAsciiStl()){
            qDebug("loadStreamAsBinStl");
            loadStreamAsBinStl();
        }
    }else{
        qDebug("loadStreamAsBinStl");
        loadStreamAsBinStl();
    }
    if(triangles.size()){
        normalize();
    }
    triOffset=QVector3D(0,0,0);
    return triangles.size();
}

int StlContainor::loadStreamAsBinStl(){
    myTriangle_t temp;
    qint32 num=0;
    int i,j;
    triangles.clear();
    stream>>num;
    if(num<0)num=MAX_TRIANGLE;
    else if(num>MAX_TRIANGLE){
        qDebug("stlContainor::loadStreamAsStlFile Num of Vector too large");
        return 0;
    }
    for(i=0;i<num;i++){
        if(stream.atEnd())break;
        for(j=0;j<3;j++)stream>>temp.normal[j];
        for(j=0;j<3;j++)stream>>temp.point[0][j];
        for(j=0;j<3;j++)stream>>temp.point[1][j];
        for(j=0;j<3;j++)stream>>temp.point[2][j];
        stream>>temp.attribute;
        //if(1 || temp.normal.length()<0.95f || temp.normal.length()>1.05f)
        {
            QVector3D v1=temp.point[0]-temp.point[1];
            QVector3D v2=temp.point[1]-temp.point[2];
            QVector3D cv=QVector3D::crossProduct(v1,v2);
            temp.normal=cv.normalized();
        }
        triangles.push_back(temp);
        //qDebug()<<temp.normal<<temp.point[0]<<temp.point[1]<<temp.point[2]<<temp.attribute;
    }
    return i;
}

int StlContainor::loadStreamAsAsciiStl(){
    triangles.clear();
    myTriangle_t temp;
    int j=0;
    QString key;
    QTextStream stl(stream.device());   //start after "solid"
    stl.seek(0);
    stl>>key;
    if(key!="solid"){
//        QMessageBox msgBox;
//        msgBox.setText("invalid Ascii Stl format");
//        msgBox.exec();
        stream.device()->seek(80);
        return 0;
    }
    stl>>key;   //useless name
    qDebug()<<"Ascii Stl inner name:"<<key;
    while(stl.atEnd()==false){
        stl>>key;
        if(key=="facet"){
            stl>>key;   //normal
            for(j=0;j<3;j++)stl>>(float&)temp.normal[j];
            stl>>key>>key;  //out loop
            stl>>key;       //vertex
            for(j=0;j<3;j++)stl>>(float&)temp.point[0][j];
            stl>>key;       //vertex
            for(j=0;j<3;j++)stl>>(float&)temp.point[1][j];
            stl>>key;       //vertex
            for(j=0;j<3;j++)stl>>(float&)temp.point[2][j];
            //if(temp.normal.length()<0.9f)
            {
                QVector3D v1=temp.point[0]-temp.point[1];
                QVector3D v2=temp.point[1]-temp.point[2];
                QVector3D cv=QVector3D::crossProduct(v1,v2);
                temp.normal=cv.normalized();
            }
            triangles.push_back(temp);
        }
    }
    return triangles.size();
}

int StlContainor::saveAsStl(QFile &f){
    if(!f.isOpen())f.open(QIODevice::Truncate | QIODevice::ReadWrite);
    if(!f.isOpen())return -1;
    stream.setDevice(&f);
    QByteArray head(80,0);
    head.insert(0,"GRAM Factory output");
    stream.writeRawData(head.data(),80);
    int size=triangles.size(),j;
    stream<<size;
    for(int i=0;i<size;i++){
        for(j=0;j<3;j++)stream<<triangles[i].normal[j];
        for(j=0;j<3;j++)stream<<triangles[i].point[0][j];
        for(j=0;j<3;j++)stream<<triangles[i].point[1][j];
        for(j=0;j<3;j++)stream<<triangles[i].point[2][j];
        stream<<triangles[i].attribute;
    }
    f.close();
    return size;
}

void StlContainor::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//       glPushMatrix();
//        nModel=MainWindow.ModelNum();
//           for(int i = 0 ; i < int nModel=1 ; i ++)
           {
           glTranslatef(triOffset[0],triOffset[1],triOffset[2]);
           int size=triangles.size();
//           glLoadName(0);
           glBegin(GL_TRIANGLES);

           glColor3d(1.0,1.0,1.0);  //底板纹理亮度
           for(int i=0;i<size;i++){
               const myTriangle_t &tri=triangles.at(i);
               glNormal3f(tri.normal[0],tri.normal[1],tri.normal[2]);
               for(quint32 j=0;j<3;j++){
                   glVertex3f(tri.point[j][0],tri.point[j][1],tri.point[j][2]);
               }
           }
           glEnd();
           }
//       glPopMatrix();
       glFlush();


}

void StlContainor::calcSizePoint(){
    int size=triangles.size();
    maxPoint=triangles.first().point[0];
    minPoint=triangles.first().point[0];
    for(int i=0;i<size;i++){
        for(int j=0;j<3;j++){
            const QVector3D &point=triangles.at(i).point[j];
            for(int k=0;k<3;k++){
                if(point[k] < minPoint[k]){
                    minPoint[k]=point[k];
                }else if(point[k] > maxPoint[k]){
                    maxPoint[k]=point[k];
                }
            }
        }
    }
}

void StlContainor::translate(QVector3D offset,bool apply){
    int size=triangles.size();
    if(size){
        if(apply==false){
            triOffset+=offset;
        }else{
            for(int i=0;i<size;i++){
                for(int j=0;j<3;j++){
                    triangles[i].point[j] += offset;
                }
            }
        }
    }
}


void StlContainor::scale(QVector3D scaler){
    int size=triangles.size();
    for(int i=0;i<size;i++){
        for(int j=0;j<3;j++){
            triangles[i].point[j] *= scaler;
        }
    }
}

void StlContainor::normalize(){
    calcSizePoint();
    QVector3D offset;
    if( fabsf(maxPoint[0] + minPoint[0]) > 0.1f){
        offset[0] = -(maxPoint[0]+minPoint[0])/2;
    }
    if( fabsf(maxPoint[1] + minPoint[1]) > 0.1f){
        offset[1] = -(maxPoint[1]+minPoint[1])/2;
    }
    if(minPoint[2] != 0){
        offset[2] = -(minPoint[2]);
    }
    translate(offset,true);
    qDebug()<<"StlContainor::normalize() : offset="<<offset;
}

void StlContainor::rotate(QVector3D rotator){
    int size=triangles.size();
    QQuaternion r;
    r*=QQuaternion::fromAxisAndAngle(1,0,0,rotator[0]);
    r*=QQuaternion::fromAxisAndAngle(0,1,0,rotator[1]);
    r*=QQuaternion::fromAxisAndAngle(0,0,1,rotator[2]);
    for(int i=0;i<size;i++){
        triangles[i].normal=r.rotatedVector(triangles[i].normal);
        for(int j=0;j<3;j++){
            triangles[i].point[j]=r.rotatedVector(triangles[i].point[j]);
        }
    }
    normalize();
}



