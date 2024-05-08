//
// Created by Administrator on 2024/5/8.
//

#ifndef TAKEFOOD_FOOD_H
#define TAKEFOOD_FOOD_H
#include <QString>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>

#define WIN_MAX     150
#define BTN_MAX     35

class Food : public QWidget, public QString {
public:
    explicit Food(QWidget *parent = nullptr,QString  name = "");
    ~Food() override;
    QPushButton *upBtn{};
    QPushButton *downBtn{};
    QString m_name;

private:
    QLabel *label{};
    QGridLayout *layout{};
};


#endif //TAKEFOOD_FOOD_H
