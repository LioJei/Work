#include <QApplication>
#include <QQuickView>
#include <QUrl>
#include "mainwindow.h"
#include "openGlDemo.h"

int main(int argc, char *argv[])
{
    //QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);
    myOpenglWidget demo;
    demo.show();
//    QQuickView* view = new QQuickView();
//    QUrl source = QUrl::fromLocalFile("../main.qml");
//    view->setSource(source);
//    view->show();
    return app.exec();
}