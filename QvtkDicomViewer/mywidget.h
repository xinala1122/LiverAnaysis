#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <openglwidget.h>
#include <QPushButton>
#include <QFileDialog>
#include <glut.h>
//#include "mainwindow.h"
class QvtkDicomViewer;
class myWidget : public QWidget
{
    Q_OBJECT
public:
    explicit myWidget(QvtkDicomViewer *mainWindow, QWidget *parent = 0);

signals:
    void signal_loadStl(QString path);
    void SigClearTree();
public slots:
    void slot_openStl();
public:
    void openOutside(QString path);

private:
    openglWidget *opengl;
    QPushButton *openstl;
    QPushButton *moveLeft;
    QPushButton *moveRight;
    QPushButton *moveUp;
    QPushButton *moveDown;
    QPushButton *clearDown;

	QvtkDicomViewer  *m_MainWindow;
    
};

#endif // MYWIDGET_H
