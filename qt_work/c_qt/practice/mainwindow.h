//
// Created by Lio on 2022/10/24.
//

#ifndef TEST_MAINWINDOW_H
#define TEST_MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <opencv2/opencv.hpp>
#include <QMainWindow>
#include <QFile>
#include <QImage>
#include <QVBoxLayout>
#include <QLabel>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include "openGlDemo.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow() override;

private:
    QPushButton *openCVBtn;
    QPushButton *openALBtn;
    QPushButton *openGLBtn;
    QPushButton *quitBtn;

private slots:
    void on_openCVBtn_clicked();
    void on_openALBtn_clicked();
    void on_openGLBtn_clicked();
};

#endif //TEST_MAINWINDOW_H
