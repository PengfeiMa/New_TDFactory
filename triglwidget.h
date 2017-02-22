#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QQuaternion>
#include "StlUtil/stlcontainor.h"
#include <typedialog.h>
#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>



//#define G1
//#define G2

class TriGLWidget : public QGLWidget
{
    Q_OBJECT

public:
    TriGLWidget(QWidget *parent = 0);
    ~TriGLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void draw(GLenum model);
    void light0UnSel();
    void light0Sel();

    void contextMenuEvent(QContextMenuEvent *event);//菜单
    QAction *copytest;//复制菜单
    QAction *roatright;
    QAction *roatleft;
    QAction *deltest;


public slots:
    void updateRotation();

    void copy_test();//复制菜单
    void roatright_test();
    void roatleft_test();
    void del_test();

    
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void updateOrtho(bool usePerspective);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);


private:
    void paintPlant();
    void paintAxis();


public:
    StlContainor triangles;
    int mode;   //0:view ; 1:translate ; 2:scale
private:
    float xRot,yRot,zRot;
    QQuaternion rotate;

    GLfloat g_lastposX,g_lastposY=0,g_lastposZ;
    
    double orthoSize,ratio;
    QPoint lastPos;
    QColor qtPurple;
    bool usePerspective;
    int width,height;
    float tranOffset[3];



public:
    int XMAX,YMAX,ZMAX; //plant width = 2*XMAX = -XMAX ~ XMAX , height=ZMAX = 0~ZMAX
    double Mod_Xsize,Mod_Ysize,Mod_Zsize;//模型尺寸
    int  STL_SIZE(double& XSize, double& YSize, double& ZSize);

    int stlAtPosition(const QPoint &pos);

};


#endif
