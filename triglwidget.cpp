#include <QtWidgets>
#include <QtOpenGL>
#include <QDebug>
#include <math.h>
#include <mainwindow.h>

#include <typedialog.h>

//#include "triglwidget.h"

#define ZOFFSET -500.0
#define YOFFSET -40.0



TriGLWidget::TriGLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    orthoSize=0.04;
    ratio=1;
    tranOffset[0]=0.0;
    tranOffset[1]=YOFFSET;
    tranOffset[2]=ZOFFSET;
    xRot=-45.0f;
    zRot=0.0f;
    qtPurple = QColor(52,150,204);
   // qtPurple = QColor(204,204,204);
    usePerspective=true;

    //XMAX=145.0f;
   // YMAX=115.0f;
    //ZMAX=170.0f;        //290*230*170

#ifdef G1
     XMAX=145.0f;
     YMAX=115.0f;
     ZMAX=170.0f;        //290*230*170
#endif

#ifdef G2
     // 180*170*130   G2
     XMAX=90.0f;
     YMAX=85.0f;
     ZMAX=130.0f;
#endif

     copytest     = new QAction(QIcon(":/icon/delete.png"), tr("复制"), this);
     connect(copytest, SIGNAL(triggered()), this, SLOT(copy_test()));
     roatright    = new QAction(QIcon(":/icon/edit.png"), tr("向右旋转10°"), this);
     connect(roatright, SIGNAL(triggered()), this, SLOT(roatright_test()));

     roatleft     = new QAction(QIcon(":/icon/delete.png"), tr("向左旋转10°"), this);
     connect(roatleft, SIGNAL(triggered()), this, SLOT(roatleft_test()));

     deltest     = new QAction(QIcon(":/icon/delete.png"), tr("删除"), this);
     connect(deltest, SIGNAL(triggered()), this, SLOT(del_test()));

     QSettings settings;
     settings.beginGroup("MainWindow");
     if(settings.value("type") == 1)
     {
         XMAX=145.0f;
         YMAX=115.0f;
         ZMAX=200.0f;        //290*230*200

     }
     else if(settings.value("type") == 2)
     {
         XMAX=90.0f;
         YMAX=80.0f;
         ZMAX=160.0f;
     }
     else if(settings.value("type") == 3)
     {
         XMAX=0.5*settings.value("valueX").toInt();
         YMAX=0.5*settings.value("valueY").toInt();
         ZMAX=settings.value("valueZ").toInt();
//         XMAX=10.0f;
//         YMAX=10.0f;
//         ZMAX=10.0f;
     }

     settings.endGroup();


}

TriGLWidget::~TriGLWidget()
{
}
void TriGLWidget::contextMenuEvent(QContextMenuEvent *event)
{
    int face = stlAtPosition(event->pos());
    if(face!=-1)
    {
            QMenu *menu = new QMenu();
            menu->setFixedWidth(150);
            menu->addAction(roatright);
            menu->addAction(roatleft);
            menu->addAction(copytest);
            menu->addAction(deltest);
            menu->exec(event->globalPos());

            delete menu;
    }
}
void TriGLWidget::copy_test()
{

}
void TriGLWidget::del_test()
{

}
void TriGLWidget::roatright_test()
{
    triangles.rotate(QVector3D(0,0,10));
}
void TriGLWidget::roatleft_test()
{
    triangles.rotate(QVector3D(0,0,-10));
}

QSize TriGLWidget::minimumSizeHint() const
{
    return QSize(480,320);
}

QSize TriGLWidget::sizeHint() const
{
    return QSize(800, 480);
}

static void qNormalizeAngle(float &angle)
{
    while (angle < 0)
        angle += 360.0f;
    while (angle > 360.0f)
        angle -= 360.0f;
}

void TriGLWidget::updateRotation(){
    if(xRot<-180.0f)xRot=-180.0f;
    else if(xRot>0.0f)xRot=0.0f;
    //qNormalizeAngle(xRot);
    qNormalizeAngle(yRot);
    qNormalizeAngle(zRot);
    updateGL();
}
GLuint texture;
void TriGLWidget::light0UnSel()
{
// 1、 光线设置
    glEnable(GL_LIGHTING);
    // 打开光照开关，告诉OpenGl使用材料属性和光照参数去决定每个顶点的颜色值，如果没有开启此参数，看到的物体是昏暗的。

    glEnable(GL_LIGHT0);
    // 启动光源0，下面设置光源的位置、环境光和物质散射光
   // glEnable(GL_MULTISAMPLE);  //使用多重采样后，点、直线和多边形的平滑特性都将被忽略。

    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
   //真正精细的透视修正。这一行告诉OpenGL我们希望进行最好的透视修正。这会十分轻微的影响性能。但使得透视图看起来好一点。

    const GLfloat lightPosition[4] = { 20, 20, 10, 1 };   // 光源位置
    const GLfloat lightAmbColor[4] = { 0.2f, 0.2f, 0.2f, 1 };  //全局的环境光
//    const GLfloat lightDiffColor1[4] = { 1.0f, 1.0f, 0.2f, 1 };  // 金黄色，选中显示物质
    const GLfloat lightDiffColor[4] = { 0.86f, 0.67f, 0.12f, 1 }; // 黄色，正常显示

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);   //设置光源位置
    glLightfv(GL_LIGHT0, GL_DIFFUSE , lightDiffColor);  //材质的光线颜色
    glLightfv(GL_LIGHT0, GL_AMBIENT , lightAmbColor);   // 环境光的颜色

//     glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE , lightDiffColor);
     // 同时设置GL_AMBIENT和GL_DIFFUSE属性，取相同的值，可以达到比较真实的效果。

     //增加其他的glLightxx设置可以设置光源类型和颜色

//2、材料属性设置

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE); //设置双面光照，使物体的背面接受的光照准确


    glEnable(GL_COLOR_MATERIAL);    //使能材料颜色跟踪,告诉OpenGL仅仅通过调用glColor来设置材料属性，可以反应材料的颜色

    glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);   //光线跟踪类型,环境光和散射光

    glColorMaterial(GL_BACK,GL_AMBIENT_AND_DIFFUSE);   //光线跟踪类型,环境光和散射光

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);   //源颜色和目标颜色混合计算
    //则表示源颜色乘以自身的alpha 值，目标颜色乘以1.0减去源颜色的alpha值，这样一来，源颜色的alpha值越大，
    //则产生的新颜色中源颜色所占比例就越大，而目标颜色所占比例则减小。这种情况下，
    //我们可以简单的将源颜色的alpha值理解为“不透明度”。
    //所谓源颜色和目标颜色，是跟绘制的顺序有关的。假如先绘制了一个红色的物体，再在其上绘制绿色的物体。
    //则绿色是源颜色，红色是目标颜色。如果顺序反过来，则 红色就是源颜色，绿色才是目标颜色。
    //在绘制时，应该注意顺序，使得绘制的源颜色与设置的源因子对应，目标颜色与设置的目标因子对应。

}

void TriGLWidget::light0Sel()
{

    const GLfloat lightDiffColor[4] = { 1.0f, 1.0f, 0.2f, 1 };  // 金黄色，选中物质

    glLightfv(GL_LIGHT0, GL_DIFFUSE , lightDiffColor);
   // glLightfv(GL_LIGHT0, GL_AMBIENT , lightAmbColor);
    //增加其他的glLightxx设置可以设置光源类型和颜色

}


void TriGLWidget::initializeGL()
{
    qglClearColor(qtPurple);
    //开启深度测试,减少绘图时间
    glEnable(GL_DEPTH_TEST);
    //开启正面检测(方式),
    //glEnable(GL_CULL_FACE);
    //glEnable(GL_FRONT_FACE);
    //glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_2D);

    glShadeModel(GL_SMOOTH);  //平滑着色，图元的内部颜色是每个顶点颜色的平滑过渡

    light0UnSel();

    //texture
    uint8_t pixels[16]={107,173,197, 108,191,220,0,0,\
                        108,191,220, 107,173,197};
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);    //ref light
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0,GL_RGB, GL_UNSIGNED_BYTE, pixels);


}

void TriGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //清除viewport的缓冲区
    draw(GL_RENDER);    
}

void TriGLWidget::draw(GLenum model)
{
    if(model==GL_SELECT)
       {
           glMatrixMode(GL_MODELVIEW);

           glPushMatrix();
           glLoadName(0);
           glLoadIdentity();
//           glTranslatef(tranOffset[0],tranOffset[1],tranOffset[2]);
//           glRotatef(xRot,1,0,0);
//           glRotatef(zRot,0,0,1);
           {
               glEnable(GL_LIGHTING);
               {                  
                   triangles.paintGL();//模型
               }
               glDisable(GL_LIGHTING);
           }

           glPopMatrix();
       }
       else if(model==GL_RENDER)
       {
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();//
            glTranslatef(tranOffset[0],tranOffset[1],tranOffset[2]);
            glRotatef(xRot,1,0,0);
 //           glRotatef(zRot,0,0,1);
           {
               glEnable(GL_LIGHTING);
               {                   
                   triangles.paintGL();//模型
               }
               glDisable(GL_LIGHTING);
           }
           glPopMatrix();

           glMatrixMode(GL_MODELVIEW);
           glPushMatrix();
           glLoadIdentity();
           glTranslatef(0,YOFFSET,ZOFFSET);
           glRotatef(xRot,1,0,0);
//           glRotatef(zRot,0,0,1);
          // glTranslatef(0,-40,-500);
           glEnable(GL_BLEND);
           paintPlant();    //包围盒
           glDisable(GL_BLEND);
           paintAxis();  //小坐标系
             glPopMatrix();


       }
    else {
        ;
    }
}

void TriGLWidget::updateOrtho(bool usePerspective){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(usePerspective){
        //gluPerspective(orthoSize*300,1,0.1,1000);
        glFrustum(-orthoSize, orthoSize, -orthoSize*ratio, orthoSize*ratio, 0.1, 1000.0);

       // gluLookAt(0,0,500,0,0,-500,0,1,0);
    }else{
        glOrtho(-orthoSize, orthoSize, -orthoSize*ratio, orthoSize*ratio, 0.1, 1000.0);
      //  gluLookAt(0,0,500,0,0,-500,0,1,0);
    }
    glMatrixMode(GL_MODELVIEW);
}

void TriGLWidget::resizeGL(int width, int height)
{
    //int side = qMin(width, height);
    //glViewport((width - side) / 2, (height - side) / 2, side, side);
    this->height=height;
    this->width=width;
    glViewport(0,0,width,height);
    ratio=(double)(height)/(double)(width); 
    updateOrtho(usePerspective);

updateGL();
    //qDebug()<<"resizeGL()"<<width<<","<<height<<" r="<<ratio;
}
//! [8]

//! [9]
void TriGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
//    if(event->button()==Qt::LeftButton)
//        {
//            int face = stlAtPosition(event->pos());
//         if(face==0)
//         {
//             qDebug() << lastPos;
//             QVector3D r;
//             r[0]=10;
//             r[1]=10;
//             r[2]=10;
//            triangles.translate(r);
//         }
//        }
}

void TriGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    float dx = event->x() - lastPos.x();
    float dy = event->y() - lastPos.y();

    float rotateSpeed=orthoSize*15.0f;
    rotateSpeed=rotateSpeed<0.3?0.3:rotateSpeed;
    rotateSpeed=rotateSpeed>1.5?1.5:rotateSpeed;
    float delta[3]={
        dy*0.5f*rotateSpeed,
        0,
        dx*0.5f*rotateSpeed,
    };

    if (event->buttons() & Qt::RightButton) {
        if(mode==2){
            float scaler=1.0f+dx*0.01f-dy*0.01f;
            triangles.scale(QVector3D(scaler,scaler,scaler));
            updateGL();
        }else if(mode==1){
            triangles.translate(QVector3D(dx*orthoSize*14,-dy*orthoSize*14,0));
            updateGL();
        }else{
            xRot+=delta[0];
            yRot+=delta[1];
            zRot+=delta[2];
            updateRotation();

        }
    }else if (event->buttons() & Qt::LeftButton) {
        int face = stlAtPosition(event->pos());
     if(face==0)
        {
//        GLint viewport[4];
//        GLdouble modelview[16];
//        GLdouble projection[16];
//        GLfloat winX, winY;
//        GLdouble near_posX, near_posY, near_posZ;
//        GLdouble far_posX, far_posY, far_posZ;
//        GLfloat vecX,vecY,vecZ;
//        glGetIntegerv(GL_VIEWPORT, viewport);
//        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
//        glGetDoublev(GL_PROJECTION_MATRIX, projection);

//        winX = event->pos().x();
//        winY = viewport[3] - event->pos().y();

//        //        glReadPixels((int)winX, (int)winY, 1 , 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
//        gluUnProject(winX, winY, 0.0f, modelview, projection, viewport, &near_posX, &near_posY, &near_posZ);
//        gluUnProject(winX, winY, 1.0f, modelview, projection, viewport, &far_posX, &far_posY, &far_posZ);


//        vecX=far_posX-near_posX;
//        vecY=far_posY-near_posY;
//        vecZ=far_posZ-near_posZ;


////        if(dragFlag[0]==1)//选中物体0，movepos_x[0]，movepos_y[0]为鼠标坐标转换为模型坐标的值
////        {
//            triangles.triOffset[0]=vecX*(g_lastposY-near_posY)/vecY+near_posX;
//            triangles.triOffset[1]=-vecZ*(g_lastposY-near_posY)/vecY+near_posZ-500;


//    //        qDebug() <<  xRot;

//            STL_SIZE(Mod_Xsize,Mod_Ysize,Mod_Zsize);

//            if(triangles.triOffset[0]>XMAX)
//                triangles.triOffset[0]=XMAX-0.5*Mod_Xsize;
//            if(triangles.triOffset[0]<-XMAX)
//                triangles.triOffset[0]=-XMAX+0.5*Mod_Xsize;
//            if(triangles.triOffset[1]>YMAX)
//                triangles.triOffset[1]=YMAX-0.5*Mod_Ysize;
//            if(triangles.triOffset[1]<-YMAX)
//                triangles.triOffset[1]=-YMAX+0.5*Mod_Ysize;
//            qDebug() <<  event->pos();


         QVector3D  offset;
         offset[0]=dx*orthoSize*5;
         offset[1]=-dy*orthoSize*14;
         offset[2]=0;

//         STL_SIZE(Mod_Xsize,Mod_Ysize,Mod_Zsize);
//         if(triangles.triOffset[0]>XMAX)
//             triangles.triOffset[0]=XMAX-Mod_Xsize;
//         if(triangles.triOffset[0]<-XMAX)
//             triangles.triOffset[0]=-XMAX+Mod_Xsize;
//         if(triangles.triOffset[1]>YMAX)
//             triangles.triOffset[1]=YMAX-Mod_Ysize;
//         if(triangles.triOffset[1]<-YMAX)
//             triangles.triOffset[1]=-YMAX+Mod_Ysize;
//            triangles.translate(offset);
        triangles.triOffset[0]+=offset[0];
        triangles.triOffset[1]+=offset[1];
                 STL_SIZE(Mod_Xsize,Mod_Ysize,Mod_Zsize);
                 if(triangles.triOffset[0]>XMAX-0.5*Mod_Xsize)
                     triangles.triOffset[0]=XMAX-0.5*Mod_Xsize;
                 if(triangles.triOffset[0]<-XMAX+0.5*Mod_Xsize)
                     triangles.triOffset[0]=-XMAX+0.5*Mod_Xsize;
                 if(triangles.triOffset[1]>YMAX-0.5*Mod_Ysize)
                     triangles.triOffset[1]=YMAX-0.5*Mod_Ysize;
                 if(triangles.triOffset[1]<-YMAX+0.5*Mod_Ysize)
                     triangles.triOffset[1]=-YMAX+0.5*Mod_Ysize;




        updateGL();


//        }
        }

    }

//    g_lastposX=triangles.triOffset[0];
    g_lastposY= -40;
//    g_lastposZ=triangles.triOffset[2];

    lastPos = event->pos();
}

void TriGLWidget::wheelEvent(QWheelEvent *event){
     if (event->orientation() == Qt::Vertical){
         {
             if(event->delta()>0){
                 orthoSize*=0.8;
             }else{
                 orthoSize*=1.2;
             }
             if(orthoSize<=0.01){
                 orthoSize=0.01;
             }else if(orthoSize>0.2){
                 orthoSize=0.2;
             }
             //qDebug()<<"orthoSize="<<orthoSize;
             updateOrtho(usePerspective);
         }
         updateGL();
     }
     event->accept();
}

void TriGLWidget::paintAxis(){
    //change projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-width/2, width/2, -height/2, height/2, 0.1, 1000.0);
    
    //
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(-width/2+50,-height/2+50,-500);
    glRotatef(xRot,1,0,0);
//    glRotatef(zRot,0,0,1);
    
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3d(1.0,0,0);
    glVertex3d(0,0,0);
    glVertex3d(25,0,0);
    glColor3d(0,1.0,0);
    glVertex3d(0,0,0);
    glVertex3d(0,25,0);
    glColor3f(0,0,1.0);
    glVertex3d(0,0,0);
    glVertex3d(0,0,25);
    glEnd();
    glDisable(GL_DEPTH_TEST);
    qglColor(Qt::lightGray);

    renderText(25,0,0,"X");
    renderText(0,25,0,"Y");
    renderText(0,0,25,"Z");
    glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void TriGLWidget::paintPlant(){
#define SCALE 20
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUAD_STRIP);
        //glColor4f(0.4f,0.4f,0.7f,0.6f);
        glTexCoord2f( XMAX/SCALE, YMAX/SCALE);glVertex3f(XMAX,YMAX,0);
        glTexCoord2f(-XMAX/SCALE, YMAX/SCALE);glVertex3f(-XMAX,YMAX,0);
        glTexCoord2f( XMAX/SCALE,-YMAX/SCALE);glVertex3f(XMAX,-YMAX,0);
        glTexCoord2f(-XMAX/SCALE,-YMAX/SCALE);glVertex3f(-XMAX,-YMAX,0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glBegin(GL_LINE_STRIP);
        glColor4f(0.1f,0.1f,0.1f,0.5f);
        glVertex3f(XMAX,YMAX,ZMAX);
        glVertex3f(-XMAX,YMAX,ZMAX);
        glVertex3f(-XMAX,-YMAX,ZMAX);
        glVertex3f(XMAX,-YMAX,ZMAX);
        glVertex3f(XMAX,YMAX,ZMAX);
        glVertex3f(XMAX,YMAX,0);
        glVertex3f(-XMAX,YMAX,0);
        glVertex3f(-XMAX,-YMAX,0);
        glVertex3f(XMAX,-YMAX,0);
        glVertex3f(XMAX,YMAX,0);
    glEnd();
    glBegin(GL_LINES);
        glColor4f(0.1f,0.1f,0.1f,0.5f);
        glVertex3f(XMAX,YMAX,0);
        glVertex3f(XMAX,YMAX,ZMAX);
        glVertex3f(-XMAX,YMAX,0);
        glVertex3f(-XMAX,YMAX,ZMAX);
        glVertex3f(XMAX,-YMAX,0);
        glVertex3f(XMAX,-YMAX,ZMAX);
        glVertex3f(-XMAX,-YMAX,0);
        glVertex3f(-XMAX,-YMAX,ZMAX);
    glEnd();
}

int TriGLWidget::stlAtPosition(const QPoint &pos)
{
    int hits = 0;
    const int MaxSize = 512;
    GLuint buffer[MaxSize];
    GLint viewport[4];
    makeCurrent();
    glGetIntegerv(GL_VIEWPORT, viewport);
    glSelectBuffer(MaxSize, buffer);
    glRenderMode(GL_SELECT);

    glInitNames();//初始化名称堆栈
    glPushName(0);//将名称0推入栈顶
    glMatrixMode(GL_PROJECTION);//将之后的矩阵操作应用到投影矩阵堆栈
    glPushMatrix();
    {
        glLoadIdentity();

        gluPickMatrix(GLdouble(pos.x()), GLdouble(viewport[3] - pos.y()), 5.0, 5.0, viewport);

     //  gluPerspective(45.0, GLfloat(viewport[2]/viewport[3]), 1.0, 500.0);
        updateOrtho(usePerspective);

        glMatrixMode(GL_MODELVIEW);

        draw(GL_SELECT);
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    hits = glRenderMode(GL_RENDER);
//    if(!glRenderMode(GL_RENDER))
//       return -1;
    if(hits == 0)
        return -1;

    return buffer[3];
}
void TriGLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
//    int face = stlAtPosition(event->pos());
//    if(face > -1)
//    {
//         light0Sel();
//         updateOrtho(usePerspective);
//         updateGL();
//     }
//    else
//    {
//        light0UnSel();
//        updateOrtho(usePerspective);
//        updateGL();
//    }
}

int  TriGLWidget::STL_SIZE(double& XSize, double& YSize, double& ZSize)
{

    int num=this->triangles.getNum();

    if (num == 0)

        return 0;

    // initialize parameters that will be used to store the minimum and maximum extents of the geometry
    // described by the STL file
    double x_min,x_max,y_min,y_max,z_min,z_max;
    x_min = 1e+30, x_max = -1e+30;
    y_min = 1e+30, y_max = -1e+30;
    z_min = 1e+30, z_max = -1e+30;

    QVector<myTriangle_t> tri = this->triangles.triangles;

    for(int i=0;i<num;i++)
    {
        // read the three components of the normal vector

        // update geometry extents along X, Y and Z based on vertex 1 of this triangle
        if (tri[i].point[0].x() < x_min)
            x_min = tri[i].point[0].x();

        if (tri[i].point[0].x() > x_max)
            x_max = tri[i].point[0].x();

        if (tri[i].point[0].y() < y_min)
            y_min = tri[i].point[0].y();

        if (tri[i].point[0].y() > y_max)
            y_max = tri[i].point[0].y();

        if (tri[i].point[0].z() < z_min)
            z_min = tri[i].point[0].z();

        if (tri[i].point[0].z() > z_max)
            z_max = tri[i].point[0].z();

        // update geometry extents along X, Y and Z based on vertex 2 of this triangle
        if (tri[i].point[1].x() < x_min)
            x_min = tri[i].point[1].x();

        if (tri[i].point[1].x() > x_max)
            x_max = tri[i].point[1].x();

        if (tri[i].point[1].y() < y_min)
            y_min = tri[i].point[1].y();

        if (tri[i].point[1].y() > y_max)
            y_max = tri[i].point[1].y();

        if (tri[i].point[1].z() < z_min)
            z_min = tri[i].point[1].z();

        if (tri[i].point[1].z() > z_max)
            z_max = tri[i].point[1].z();

        // update geometry extents along X, Y and Z based on vertex 3 of this triangle
        if (tri[i].point[2].x() < x_min)
            x_min = tri[i].point[2].x();

        if (tri[i].point[2].x() > x_max)
            x_max = tri[i].point[2].x();

        if (tri[i].point[2].y() < y_min)
            y_min = tri[i].point[2].y();

        if (tri[i].point[2].y() > y_max)
            y_max = tri[i].point[2].y();

        if (tri[i].point[2].z() < z_min)
            z_min = tri[i].point[2].z();

        if (tri[i].point[2].z() > z_max)
            z_max = tri[i].point[2].z();
    }
    XSize = x_max - x_min;
    YSize = y_max - y_min;
    ZSize = z_max - z_min;

    return 1;
}




