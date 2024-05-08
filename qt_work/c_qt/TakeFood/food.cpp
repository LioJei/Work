//
// Created by Administrator on 2024/5/8.
//

#include "food.h"

#include <utility>

Food::Food(QWidget *parent, QString  name) : m_name(std::move(name)){
    this->setMaximumSize(WIN_MAX,WIN_MAX);
    layout = new QGridLayout(this);
    label = new QLabel(m_name);
    label->setAlignment(Qt::AlignCenter);
    upBtn = new QPushButton("+");
    upBtn->setMaximumSize(BTN_MAX,BTN_MAX);
    downBtn = new QPushButton("-");
    downBtn->setMaximumSize(BTN_MAX,BTN_MAX);
    downBtn->setDisabled(true);
    layout->addWidget(label,0,0,1,2);
    layout->addWidget(downBtn, 1,0);
    layout->addWidget(upBtn,1,1);
}

Food::~Food() = default;
