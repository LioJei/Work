//
// Created by Administrator on 2024/2/5.
//

#ifndef TEST_DEMO_H
#define TEST_DEMO_H

///头文件声明
#include "headfile.h"
///定义声明
//列举分割后字符串容器前N项
#define SPLIT_SHOW_NUMBER   4
#define STR2CHR(x)          (std::to_string(x).c_str())
///枚举声明
enum E_calcCh{
    e_add = 0,
    e_sub,
    e_mul,
    e_div,
    e_mod,
    e_max,
    e_min,
};

class TestDemo {
public:
    /**
     * @brief:      构造函数
     * @param[in]:  name(实例名)
     * */
    explicit TestDemo(const char* name);
    /**
     * @brief:      析构函数
     * */
    ~TestDemo();
    /**
     * @brief:      实例打印函数
     * @param[in]:  msg(需打印的字符串)
     * @return:     None
     * */
    void tPrint(const char* msg);
    /**
     * @brief:      字符串分割函数
     * @param[in]:  str(需分割的字符串)
     * @param[in]:  ch(分隔符号)
     * @return:     vec(分割完成的string容器)
     * */
    static std::vector<std::string> m_split(std::string str, const char* ch);

private:
    const char* m_name; //实例名
};

template <typename Type>
auto calcFunc(Type tVal1, Type tVal2, E_calcCh ch) -> decltype(tVal1){
    switch (ch) {

        case e_add:
            return (tVal1+tVal2);
        case e_sub:
            return (tVal1-tVal2);
        case e_mul:
            return (tVal1*tVal2);
        case e_div:
            return (tVal1/tVal2);
        case e_mod:
            return (tVal1%tVal2);
        case e_max:
            return (tVal1>tVal2?tVal1:tVal2);
        case e_min:
            return (tVal1<tVal2?tVal1:tVal2);
        default:
            break;
    }
}
#endif //TEST_DEMO_H
