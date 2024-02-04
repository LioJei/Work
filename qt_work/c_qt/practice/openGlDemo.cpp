#include "openGlDemo.h"
#include <iostream>
#include "GL/glut.h"
using namespace std;
myOpenglWidget::myOpenglWidget(QWidget *parent) : QOpenGLWidget(parent)
{

}

void myOpenglWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0, 1.0, 1.0,1.0);
}

void myOpenglWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.5,1.5,-1.5,1.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void myOpenglWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPointSize(10.0);
    glColor3d(1.0,0.0,0.0);
    glBegin(GL_POINTS);
    glVertex3d(0.0, 0.0, 0.0);
    glEnd();
}