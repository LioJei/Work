#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    qSetMessagePattern("[%{time yyyy/MM/dd h:mm:ss:zzz}"
                       " %{if-debug}Debug%{endif}"
                       "%{if-info}Info%{endif}"
                       "%{if-warning}Warning%{endif}"
                       "%{if-critical}Critical%{endif}"
                       "%{if-fatal}Fatal%{endif}] -"
                       "%{message}");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
