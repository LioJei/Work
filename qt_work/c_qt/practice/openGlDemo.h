#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
class myOpenglWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
Q_OBJECT
public:
    explicit myOpenglWidget(QWidget *parent = nullptr);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
signals:

public slots:
};

#endif // MYOPENGLWIDGET_H