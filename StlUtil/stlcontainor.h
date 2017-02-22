#pragma once

#include <QObject>
#include <QVector>
#include <QVector3D>
#include <stdint.h>
#include <QDataStream>
#include <QFile>
#include <gl/GLU.h>




typedef struct{
    QVector3D normal;
    QVector3D point[3];
    quint16 attribute;
}myTriangle_t;


class StlContainor : public QObject
{
    Q_OBJECT
public:
    StlContainor();
    int loadStreamAsStlFile();
    int loadStreamAsBinStl();
    int loadStreamAsAsciiStl();
    int saveAsStl(QFile &f);

    
    void paintGL();
    
    void append(StlContainor &stl);
    void scale(QVector3D scaler);
    void translate(QVector3D offset, bool apply=false);
    void calcSizePoint();
    void clear(){
        triangles.clear();
    }
    int getNum(){
        return triangles.size();
    }
    void normalize();   //calcSizePoint & balance the min/max
    void rotate(QVector3D rotator);

public:
    QVector3D minPoint,maxPoint,triOffset;
    QDataStream stream;
    QVector<myTriangle_t> triangles;
};





