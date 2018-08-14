#include "camera.h"
#include <GL/glu.h>


Camera::Camera()   {}

Camera::~Camera()  {}


void Camera::setCamera( float eyeX, float eyeY, float eyeZ,
                        float lookX, float lookY, float lookZ,
                        float upX, float upY, float upZ)
{

    eye.set(eyeX, eyeY, eyeZ);
    look.set(lookX, lookY, lookZ);
    up.set(upX, upY, upZ);
    Vector3 upvec(up.x-eye.x,up.y-eye.y,up.z-eye.z);


    n.set(eye.x-look.x, eye.y-look.y, eye.z-look.z);
    u.set(upvec.cross(n).x,upvec.cross(n).y,upvec.cross(n).z);
    v.set(n.cross(u).x,n.cross(u).y,n.cross(u).z);

    u.normalize();
    v.normalize();
    n.normalize();
    setModelViewMatrix();
}

float Camera::getDist()
{
    float dist=pow(eye.x,2)+pow(eye.y,2)+pow(eye.z,2);
    return pow(dist,0.5);
    //return 100.0;
}

void Camera::setModelViewMatrix()
{
    float m[16];
    Vector3 eVec(eye.x, eye.y,eye.z);
    m[0]=u.x; m[4]=u.y; m[8]=u.z; m[12]=-eVec.dot(u);
    m[1]=v.x; m[5]=v.y; m[9]=v.z; m[13]=-eVec.dot(v);
    m[2]=n.x; m[6]=n.y; m[10]=n.z; m[14]=-eVec.dot(n);
    m[3]=0;  m[7]=0;  m[11]=0;  m[15]=1.0;
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m);     //
}

void Camera::roll(float angle)
{
    float cs=cos(angle*3.14159265/180);
    float sn=sin(angle*3.14159265/180);
    Vector3 t(u);
    Vector3 s(v);
    u.set(cs*t.x-sn*s.x, cs*t.y-sn*s.y, cs*t.z-sn*s.z);
    v.set(sn*t.x+cs*s.x, sn*t.y+cs*s.y, sn*t.z+cs*s.z);
    setModelViewMatrix();          //
}

void Camera::yaw(float angle)
{
    float cs=cos(angle*3.14159265/180);
    float sn=sin(angle*3.14159265/180);
    Vector3 t(n);
    Vector3 s(u);
    n.set(cs*t.x-sn*s.x, cs*t.y-sn*s.y, cs*t.z-sn*s.z);
    u.set(sn*t.x+cs*s.x, sn*t.y+cs*s.y, sn*t.z+cs*s.z);
    setModelViewMatrix();
}

void Camera::pitch(float angle)
{
    float cs=cos(angle*3.14159265/180);
    float sn=sin(angle*3.14159265/180);
    Vector3 t(v);
    Vector3 s(n);
    v.set(cs*t.x-sn*s.x, cs*t.y-sn*s.y, cs*t.z-sn*s.z);
    n.set(sn*t.x+cs*s.x, sn*t.y+cs*s.y, sn*t.z+cs*s.z);
    setModelViewMatrix();
}

void Camera::slide(float du, float dv, float dn)
{
    eye.x+=du*u.x+dv*v.x+dn*n.x;
    eye.y+=du*u.y+dv*v.y+dn*n.y;
    eye.z+=du*u.z+dv*v.z+dn*n.z;
    look.x+=du*u.x+dv*v.x+dn*n.x;
    look.y+=du*u.y+dv*v.y+dn*n.y;
    look.z+=du*u.z+dv*v.z+dn*n.z;
    setModelViewMatrix();
}

void Camera::setShape(float viewAngle, float aspect, float Near, float Far)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                                   //
    gluPerspective(viewAngle,aspect, Near, Far);        //
}
