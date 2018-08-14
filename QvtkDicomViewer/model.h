#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <face.h>
#include <QList>
#include <QFile>
#include <QMessageBox>
#include <QtOpenGL>
#include <GL/glu.h>
#include<glut.h>
typedef struct modelSize{
    //
    //
    float smallX;
    float largeX;
    float smallY;
    float largeY;
    float smallZ;
    float largeZ;
} modelSize;

class STLModel : public QObject
{
    Q_OBJECT
public:
    explicit STLModel(QObject *parent = 0);
private:
    QList<face*> faceList;
    modelSize size;
    QVector3D center;
public:
    void model_load(QString path);
    void model_readText(QString path);
    void model_readBinary(QString path);
    void model_clear(bool clearAll = false);
    void model_draw();

    void model_test();
    modelSize model_getSize();
    void model_getSizeWhileReadPoint(QVector3D point);
    QVector3D model_getCenter(modelSize size);

    int model_getFacesCount();
private:
    QVector3D getCoordinateFromString(QString line);
signals:
    void signal_finishLoad();
    
public slots:
    
};

#endif // MODEL_H
