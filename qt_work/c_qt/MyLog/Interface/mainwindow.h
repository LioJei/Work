//
// Created by 14621 on 2022/8/22 0022.
//

#ifndef TEST_MAINWINDOW_H
#define TEST_MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    void init_interface();

    void setStyle(const QString &filePath);

private slots:

};

#endif //TEST_MAINWINDOW_H
