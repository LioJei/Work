//
// Created by Administrator on 2024/5/8.
//

#include "mainwindow.h"

MainWindow::MainWindow(QMainWindow *parent) {
    this->setWindowTitle("❤️❤️❤️❤️超级无敌漂亮海霞点餐菜谱❤️❤️❤️❤️");
    status = new QStatusBar(this);
    status->setObjectName("状态栏");
    status->setStyleSheet("QStatusBar::item{border: 0px}");
    this->setStatusBar(status);
    auto *statusLabel = new QLabel("❤️❤️❤️❤️❤️❤️❤️❤️❤️❤️❤");
    status->showMessage(QString("版本号：v1.1"));
    status->addPermanentWidget(statusLabel);
    initial();
}

void MainWindow::on_sendMsgBtn_clicked() {
    sendMsgBtn->setDisabled(true);
    bool result = m_sender->setMsg("菜单", showStr);
    if (result)
        QMessageBox::information(this, tr("小李报告！"), tr("点餐已收到，正在积极备餐！"));
    else
        QMessageBox::information(this, tr("小李报告！"), tr("没有发出去捏！是不是网络波动呀"));
    sendMsgBtn->setEnabled(true);
}

void MainWindow::initial() {
    ///成员初始化与界面配置
    m_sender = new Sender(tr("1462134792@qq.com"), tr("1462134792@qq.com"), tr("kbrdgdlrjhhsieeb"), tr("smtp.qq.com"),
                          25);
    addFood();
    mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);
    mainLayout = new QGridLayout(mainWidget);
    sendMsgBtn = new QPushButton(tr("一键发送,尽享美味"));
    sendMsgBtn->setMaximumSize(140, 40);
    addFoodBtn = new QPushButton(tr("添加菜谱"));
    addFoodBtn->setMaximumSize(80, 40);
    addFoodBtn->hide();
    addFoodEdt = new QLineEdit;
    showEdt = new QTextEdit;
    addFoodEdt->setMaximumSize(150,30);
    showEdt->setText(showStr);
    showEdt->setReadOnly(true);
    ///菜单初始化
    int row = 0, colum = 0;
    for (auto iter: vecFood) {
        mainLayout->addWidget(iter, row, colum);
        conFunc(iter);
        if (colum++ == 4) {
            colum = 0;
            row++;
        }
    }
    row++;
    ///界面排版
    mainLayout->addWidget(showEdt, row, 0, 2, 3);
    mainLayout->addWidget(addFoodEdt, row, 4, 1, 1);
    mainLayout->addWidget(addFoodBtn, row++, 5, 1, 1);
    mainLayout->addWidget(sendMsgBtn, row, 4, 1, 2);
    mainLayout->setSpacing(5);
    mainLayout->setMargin(10);
    ///信号槽链接
    connect(sendMsgBtn, SIGNAL(clicked()), this, SLOT(on_sendMsgBtn_clicked()));
    connect(addFoodEdt, &QLineEdit::textChanged, addFoodBtn, &QPushButton::show);
    connect(addFoodBtn, &QPushButton::clicked, this, &MainWindow::on_addFoodBtn_clicked);
}

void MainWindow::updateStr(ChoiceCode code, const QString &str) {
    showStr.clear();
    showStr.append("已点:\r\n");
    int index = vector.indexOf(str);
    int i = 0;
    switch (code) {
        case addCode:
            vector.append(str);
            for (const auto &iter: vector) {
                showStr.append(QString(iter + " "));
                if (i++ == 3) {
                    showStr.append("\r\n");
                    i = 0;
                }

            }
            break;
        case subCode:
            vector.erase(vector.begin() + index);
            for (const auto &iter: vector) {
                showStr.append(QString(iter + " "));
                if (i++ == 3) {
                    showStr.append("\r\n");
                    i = 0;
                }
            }
            break;
        case askCode:
            vector.append(str);
            for (const auto &iter: vector) {
                showStr.append(QString(iter + " "));
                if (i++ == 3) {
                    showStr.append("\r\n");
                    i = 0;
                }
            }
            break;
    }
    showEdt->setText(showStr);
}

void MainWindow::conFunc(Food *food) {
    connect(food->upBtn, &QPushButton::clicked, this, [=]() {
        food->upBtn->setDisabled(true);
        food->downBtn->setEnabled(true);
        updateStr(addCode, food->m_name);
    });
    connect(food->downBtn, &QPushButton::clicked, this, [=]() {
        food->downBtn->setDisabled(true);
        food->upBtn->setEnabled(true);
        updateStr(subCode, food->m_name);
    });
}

void MainWindow::addFood() {
    vecFood.push_back(new Food(nullptr, "土豆烧牛肉"));
    vecFood.push_back(new Food(nullptr, "炖烧排骨"));
    vecFood.push_back(new Food(nullptr, "水煮肉片"));
    vecFood.push_back(new Food(nullptr, "香辣红烧肉"));
    vecFood.push_back(new Food(nullptr, "辣椒炒肉"));
    vecFood.push_back(new Food(nullptr, "蜜汁鸡翅"));
    vecFood.push_back(new Food(nullptr, "辣子鸡丁"));
    vecFood.push_back(new Food(nullptr, "西红柿炒蛋"));
    vecFood.push_back(new Food(nullptr, "酸辣土豆丝"));
    vecFood.push_back(new Food(nullptr, "煎蛋汤"));
    vecFood.push_back(new Food(nullptr, "番茄鸡蛋汤"));
}

void MainWindow::on_addFoodBtn_clicked() {
    updateStr(askCode, QString("\r\n+++添加菜谱+++\r\n<<<<< "+addFoodEdt->text())+ " >>>>>\r\n++++++++++++++\r\n");
}

MainWindow::~MainWindow() = default;