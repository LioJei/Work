/**
* Copyright (c) 2025 cdly.Co.,Ltd. All rights reserved.
* @brief Gpio控制接口
* @author lijunjie
* @date 2025/4/10
*/
#ifndef RKPROJ_GPIOCTRL_H
#define RKPROJ_GPIOCTRL_H

#include "headfile.h"

class GpioCtrl {
public:
    /**
     * @brief: 初始化构造，声明pin脚，并且导出该io脚
     * @param[in]: pin(所需控制的io脚)
     * */
    explicit GpioCtrl(const char *pin);

    /**
     * @brief: 析构函数，在析构里面解导出io脚
     * */
    ~GpioCtrl();

    /**
     * @brief: 设置pin脚输入输出
     * @param[in]: direction(in/out)
     * */
    void setDirection(const char *direction);

    /**
   * @brief: 读取pin脚输入输出状态
   * @return: 输入输出状态(in/out)
   * */
    const char *readDirection();

    /**
    * @brief: 设置pin脚高低电平
    * @param[in]: level(0/1)
    * */
    void setValue(const char *level);

    /**
    * @brief: 读取pin脚高低电平
    * @return: 电平状态(0/1)
    * */
    char readValue();

private:
    const char *m_pin;
};

#endif //RKPROJ_GPIOCTRL_H
