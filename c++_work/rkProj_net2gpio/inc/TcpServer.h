/**
* Copyright (c) 2025 cdly.Co.,Ltd. All rights reserved.
* @brief Tcp数据交互接口
* @author lijunjie
* @date 2025/4/10
*/

#ifndef RKPROJ_TCPSERVER_H
#define RKPROJ_TCPSERVER_H

#include "headfile.h"

#define MAX_CLIENTS 10
#define NAME_SIZE   20
#define RECVPORT    8080
#define SENDPORT    8888
#define RECVIP      "192.168.153.144"
#define SENDIP      "127.0.0.1"

//TODO:数据结构待定
struct Data {
    int id;
    float value;
    char name[NAME_SIZE];
};

class TCPServer {
public:
    /**
     * @brief: 初始化构造，声明上层ip和端口，初始化服务端socket
     * @param[in]: ip(上层客户端静态ip地址)
     * @param[in]: port(上层客户端tcp端口)
     * */
    TCPServer(const char *ip, int port);
    /**
     * @brief: 析构函数，在析构里面释放socket相关资源
     * */
    ~TCPServer();
    /**
     * @brief: 服务端启动函数，开始通信
     * */
    void start();

private:
    int server_fd;
    struct sockaddr_in address{};
    int opt;
    int addrlen;
    int client_sockets[MAX_CLIENTS]{};
    pthread_mutex_t data_mutex{};
    /**
    * @brief: 数据流下层处理，做转发数据用，内部实现，不对外开放该接口
    * */
    static void *sendMsg(void *arg);
};


#endif //RKPROJ_TCPSERVER_H
