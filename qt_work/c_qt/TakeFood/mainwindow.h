//
// Created by Administrator on 2024/5/8.
//

#ifndef TAKEFOOD_MAINWINDOW_H
#define TAKEFOOD_MAINWINDOW_H
#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QVector>
#include <QString>
#include <QTextEdit>
#include <QLineEdit>
#include <QStatusBar>
#include "sender.h"
#include "food.h"

enum ChoiceCode{
    addCode = 0,
    subCode,
    askCode,
};

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    /**
     * @brief:      默认构造
     * */
    explicit MainWindow(QMainWindow *parent = nullptr);
    /**
     * @brief:      默认析构
     * */
    ~MainWindow() override;
    /**
     * @brief:      初始化界面
     * */
    void initial();
    /**
     * @brief:      更新菜单显示
     * @param[in]:  code--增删句柄
     * @param[in]:  str--菜名
     * */
    void updateStr(ChoiceCode code, const QString& str);
    /**
     * @brief:      初始化菜谱容器
     * */
    void addFood();
    /**
     * @brief:      菜单槽函数链接
     * @param[in]:  food--菜系实例
     * */
    void conFunc(Food *food);

private:
    QStatusBar *status{};
    QPushButton *sendMsgBtn{};
    QPushButton *addFoodBtn{};
    QLineEdit *addFoodEdt{};
    QGridLayout *mainLayout{};
    QWidget *mainWidget{};
    QTextEdit *showEdt{};
    Sender *m_sender{};
    QVector<Food*> vecFood{};
    QVector<QString> vector{};
    QString showStr{"已点:\r\n"};

private slots:
    //发送信息按钮槽
    void on_sendMsgBtn_clicked();
    void on_addFoodBtn_clicked();
};


#endif //TAKEFOOD_MAINWINDOW_H
