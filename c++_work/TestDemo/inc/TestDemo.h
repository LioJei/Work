/**
 * @author:     lio
 * @date:       2024/02/05
 * @brief:      测试类封装
 * */

#ifndef TEST_DEMO_H
#define TEST_DEMO_H

///头文件声明
#include "headfile.h"
///定义声明
#define SPLIT_SHOW_NUMBER   4   //列举分割后字符串容器前N项
#define STR2CHR(x)          (std::to_string(x).c_str()) //string转C字符
typedef int (*fptr)(int, int);  //定义运算函数指针
///枚举声明
enum E_calcCh{
    e_add = 0,
    e_sub,
    e_mul,
    e_div,
    e_mod,
    e_max,
    e_min,
};  //运算枚举

class TestDemo {
public:
    /**
     * @brief:      构造函数
     * @param[in]:  name(实例名)
     * */
    [[maybe_unused]] explicit TestDemo(const char* name);
    /**
     * @brief:      析构函数
     * */
    ~TestDemo();
    /**
     * @brief:      实例打印函数
     * @param[in]:  msg(需打印的字符串)
     * @return:     None
     * */
    [[maybe_unused]] void TestPrint(const char* msg);
    /**
     * @brief:      字符串分割函数
     * @param[in]:  str(需分割的字符串)
     * @param[in]:  ch(分隔符号)
     * @return:     vec(分割完成的string容器)
     * */
    [[maybe_unused]] static std::vector<std::string> MySplit(std::string str, const char* ch);
    /**
     * @brief:      打印各类型大小
     * @return:     None
     * */
    [[maybe_unused]] static void PrintTypeSize();
    /**
     * @brief:      函数指针测试加
     * */
    [[maybe_unused]] static int add(int val1, int val2);
    /**
    * @brief:      函数指针测试减
    * */
    [[maybe_unused]] static int sub(int val1, int val2);
    /**
     * @brief:      函数指针测试
     * */
    [[maybe_unused]] static int compute(fptr ptr, int val1, int val2);

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
