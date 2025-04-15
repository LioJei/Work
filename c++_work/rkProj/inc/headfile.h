//
// Created by lijunjie on 2025/4/10.
//

#ifndef RKPROJ_HEADFILE_H
#define RKPROJ_HEADFILE_H
//头文件包含
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
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
//数据定义
#define FILE_SIZE   (1024*1024) //1MB
#define NAME_SIZE   20
//TODO:数据结构待定
struct Data {
    int id;
    float value;
    char name[NAME_SIZE];
};

#endif //RKPROJ_HEADFILE_H
