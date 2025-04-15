//
// Created by lijunjie on 2025/4/10.
//

#ifndef RKPROJ_HEADFILE_H
#define RKPROJ_HEADFILE_H
//头文件包含
#include <cstdio>
#include <cstdlib>
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

#define FILE_NAME   "/home/lio/software/logfile.log"    //日志文件名
#define FILE_SIZE   (1024*1024)                         //最大文件大小1MB
#define NAME_SIZE   20                                  //最大通信接收字节数
//TODO:数据结构待定
//通信协议
struct Data {
    int id;
    float value;
    char name[NAME_SIZE];
};

#endif //RKPROJ_HEADFILE_H
