/**
* Copyright (c) 2025 CHENGDU LIYANG INFORMATION TECHNOLOGY Co.,Ltd. All rights reserved.
* @brief 头文件集合，全局宏、变量和接口定义
* @author lijunjie
* @date 2025/4/10
*/

#ifndef _RKPROJ_HEADFILE_H
#define _RKPROJ_HEADFILE_H

// 头文件包含
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <arpa/inet.h>
#include <sys/select.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <memory>

// 日志文件名
const std::string FILE_NAME = "/home/lio/software/logfile.log";

// 最大文件大小 1MB
constexpr size_t FILE_SIZE = 1024 * 1024;

// 最大通信接收字节数
constexpr size_t NAME_SIZE = 20;

// 通信协议
struct Data {
    int id;
    float value;
    char name[NAME_SIZE];
};

// 错误处理宏定义，用于检查系统调用的返回值
inline void check_syscall(int result, const char* call) {
    if (result == -1) {
        std::cerr << "Error in " << call << ": " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

#define CHECK_SYSCALL(call) check_syscall(call, #call)

#endif //_RKPROJ_HEADFILE_H
