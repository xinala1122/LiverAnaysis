#include "openglwidget.h"

openglWidget::openglWidget(QWidget *parent) :
    QGLWidget(parent)
{
    setMinimumHeight(50);
    setMinimumWidth(50);
    model = new STLModel(this);
    connect(model,SIGNAL(signal_finishLoad()),
            this,SLOT(slot_finishLoadStl()));
    initialOpengl();
}

void openglWidget::initialOpengl()
{   
    centerInOpenGlWid = model->model_getCenter(model->model_getSize());

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    cam.setCamera(centerInOpenGlWid.x(),centerInOpenGlWid.y(),centerInOpenGlWid.z()*5,//+100,
                  centerInOpenGlWid.x(),centerInOpenGlWid.y(),centerInOpenGlWid.z(),
                  0.0,1.0,0.0
                  );
    resizeGL(width(),height());

}

void openglWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    cam.setModelViewMatrix();
    model->model_draw();

}

void openglWidget::resizeGL(int w, int h)
{
    if (h==0)
    {
        h=1;
    }
    glViewport(0,0,w,h);


    cam.setShape(45,(GLfloat)w/h,0.1f,centerInOpenGlWid.z()*10);//200.0f);


    glMatrixMode(GL_MODELVIEW);
}

void openglWidget::initializeGL()
{
    lightingStruct whiteLighting ={
        {0.0,0.0,0.0,1.0},
        {1.0,1.0,1.0,1.0},
        {1.0,1.0,1.0,1.0},
    };

    materialStruct whiteShinyMaterial={
        {1.0,1.0,1.0,1.0},
        {1.0,1.0,1.0,1.0},
        {1.0,1.0,1.0,1.0},
        100.0
    };

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glDisable( GL_CULL_FACE);       //

    glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0,GL_AMBIENT,whiteLighting.ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,whiteLighting.diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,whiteLighting.specular);

    //

    setMaterial(&whiteShinyMaterial);

    //  glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
    glClearDepth(1.0f);									// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

}

void openglWidget::slot_recieveStl(QString path, int numOfTotal, int total)
{
    model->model_clear();
    model->model_load(path, numOfTotal, total);
}

void openglWidget::slot_finishLoadStl()
{
    initialOpengl();
    updateGL();
}

void openglWidget::slot_moveLeft()
{
    cam.slide(20,0,0);
    updateGL();

}
void openglWidget::slot_moveRight()
{
    cam.slide(-20,0,0);
    updateGL();
}

void openglWidget::slot_moveUp()
{
    cam.slide(0,-20,0);
    updateGL();
}

void openglWidget::slot_moveDown()
{
    cam.slide(0,20,0);
    updateGL();
}

void openglWidget::slot_clear()
{
    emit SigClearTree();
    model->model_clear(true);
    updateGL();

    //this->paintGL();
}

void openglWidget::setMaterial(materialStruct *material)
{
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,material->ambient);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,material->diffuse);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,material->specular);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,material->shininess);
}

void openglWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void openglWidget::mouseMoveEvent(QMouseEvent *event)
{

    int dx = event->x() - lastPos.x();
        int dy = event->y() - lastPos.y();
        if (event->buttons() & Qt::LeftButton)
        {
            RotateX(dx);
            RotateY(dy);
        }
        lastPos = event->pos();
        updateGL();
}

void openglWidget::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    if(numSteps<0)
    {
        cam.slide(0,0,-40);
    }
    else
    {
        cam.slide(0,0,40);
    }
    updateGL();
}

void openglWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Left)
        cam.slide(-5,0,0);
    else if(event->key() == Qt::Key_Right)
        cam.slide(5,0,0);
    else if(event->key() == Qt::Key_Up)
        cam.slide(0,5,0);
    else if(event->key() == Qt::Key_Down)
        cam.slide(0,-5,0);
    updateGL();
}
void openglWidget::RotateX(float angle)
{
    float d=cam.getDist();
    int cnt=100;
    float theta=angle/cnt;
    float slide_d=-2*d*sin(theta*3.14159265/360);
    cam.yaw(theta/2);
    for(;cnt!=0;--cnt)
    {
        cam.slide(slide_d,0,0);
        cam.yaw(theta);
    }
    cam.yaw(-theta/2);
}

void openglWidget::RotateY(float angle)
{
    float d=cam.getDist();
    int cnt=100;
    float theta=angle/cnt;
    float slide_d=2*d*sin(theta*3.14159265/360);
    cam.pitch(theta/2);
    for(;cnt!=0;--cnt)
    {
        cam.slide(0,slide_d,0);
        cam.pitch(theta);
    }
    cam.pitch(-theta/2);
}
