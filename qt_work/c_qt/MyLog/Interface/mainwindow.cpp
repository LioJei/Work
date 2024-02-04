//
// Created by 14621 on 2022/8/22 0022.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mainwindow.h" resolved

#include "mainwindow.h"
#include <QFile>
#include <QDebug>
#include "Logger.h"

using namespace Logger;
using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    initLog();
    MY_LOG_DEBUG<<"d";
    MY_LOG_INFO<<"i";
    MY_LOG_WARN<<"w";
    MY_LOG_CRIT<<"c";
    init_interface();
}

MainWindow::~MainWindow() {

}

void MainWindow::init_interface() {
    setStyle("W:\\work\\Digital_boards\\Test\\Style.qss");
}

void MainWindow::setStyle(const QString &filePath) {
    QFile qss(filePath);
    qss.open(QFile::ReadOnly);
    setStyleSheet(qss.readAll());
    qss.close();
}
