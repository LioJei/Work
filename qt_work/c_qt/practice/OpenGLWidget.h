#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <GL/glut.h>
#include <QDebug>

class GLWidget : public QOpenGLWidget
{
public:
    GLWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent) {
        QSurfaceFormat format;
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setVersion(4, 5); // 设置OpenGL版本为3.3
        format.setProfile(QSurfaceFormat::NoProfile); // 使用核心配置文件
        setFormat(format);
    }

protected:
    void initializeGL() override
    {
        // 初始化OpenGL状态
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // 检查OpenGL错误
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            qDebug()<< "OpenGL error:" << error;
        }
    }

    void paintGL() override
    {
        // 清除颜色缓冲区
        glClear(GL_COLOR_BUFFER_BIT);

        // 设置绘制颜色
        glColor3f(1.0f, 1.0f, 1.0f);

        // 绘制一个简单的三角形
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glEnd();
    }

    void resizeGL(int width, int height) override
    {
        // 设置视口
        glViewport(0, 0, width, height);
    }
};