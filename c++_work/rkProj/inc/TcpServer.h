/**
* Copyright (c) 2025 cdly.Co.,Ltd. All rights reserved.
* @brief Tcp数据交互接口
* @author lijunjie
* @date 2025/4/10
*/

#ifndef RKPROJ_TCPSERVER_H
#define RKPROJ_TCPSERVER_H

#include "headfile.h"
#include "Logger.h"

#define MAX_CLIENTS 10                  //最大客户端监听数
#define RECVPORT    8080                //接收端端口
#define SENDPORT    8888                //发送端端口
#define RECVIP      "192.168.153.144"   //接收端IP
#define SENDIP      "127.0.0.1"         //发送端IP

class TCPServer {
public:
    /**
     * @brief: 初始化构造，声明上层ip和端口，初始化服务端socket
     * @param[in]: ip(上层客户端静态ip地址)
     * @param[in]: port(上层客户端tcp端口)
     * @param[in]: logger(共享日志指针)
     * */
    TCPServer(const char *ip, int port, std::shared_ptr<Logger> logger);

    /**
     * @brief: 析构函数，在析构里面释放socket相关资源
     * */
    ~TCPServer();

    /**
     * @brief: 服务端启动函数，开始通信
     * */
    void start();

private:
    int server_fd;                      //套接字句柄
    struct sockaddr_in address{};       //套接字结构体
    int opt;                            //套接字配置句柄
    int addrlen;                        //套接字地址长度
    int client_sockets[MAX_CLIENTS]{};  //接收客户端套接字句柄组
    pthread_mutex_t data_mutex{};       //数据读写锁，防止冲突
    std::shared_ptr<Logger> m_logger;   //日志写入句柄

    /**
    * @brief: 数据流下层处理，做转发数据用，内部实现，不对外开放该接口
    * */
    void *sendMsg(void *arg);
};


#endif //RKPROJ_TCPSERVER_H
