#include "TcpServer.h"

TCPServer::TCPServer(const char *ip, int port) {
    opt = 1;
    addrlen = sizeof(address);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "setsockopt" << std::endl;
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        std::cerr << "bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        std::cerr << "listen" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "服务器已启动" << std::endl;
    pthread_mutex_init(&data_mutex, nullptr);
    std::fill_n(client_sockets, MAX_CLIENTS, 0);
}

TCPServer::~TCPServer() {
    close(server_fd);
    for (int client_socket: client_sockets) {
        if (client_socket > 0) {
            close(client_socket);
        }
    }
    pthread_mutex_destroy(&data_mutex);
}


void TCPServer::start() {
    int new_socket, max_sd, sd, activity;
    Data data = {0};
    pthread_t tid;
    while (true) {
        fd_set readfds;
        // 清空socket集合
        FD_ZERO(&readfds);
        // 将服务器socket添加到集合中
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // 将所有活动的客户端socket添加到集合中
        for (int client_socket: client_sockets) {
            sd = client_socket;
            // 如果有效的socket描述符，则添加到集合中
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            // 更新最大socket描述符
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // 等待活动的socket，设置超时为5秒
        struct timeval timeout{};
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        activity = select(max_sd + 1, &readfds, nullptr, nullptr, &timeout);
        if ((activity < 0) && (errno != EINTR)) {
            std::cerr << "select error" << std::endl;
            exit(EXIT_FAILURE);
        }

        // 如果有新的连接请求
        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *) &address,
                                     (socklen_t *) &addrlen)) < 0) {
                std::cerr << "accept error" << std::endl;
                exit(EXIT_FAILURE);
            }
            std::cout << "新客户端连接，socket fd:" << new_socket << std::endl;
            // 将新的socket添加到客户端数组中
            for (int &client_socket: client_sockets) {
                if (client_socket == 0) {
                    client_socket = new_socket;
                    std::cout << "客户端已添加，socket fd:" << new_socket << std::endl;
                    break;
                }
            }
        }

        // 检查每个客户端socket是否有数据可读
        for (int &client_socket: client_sockets) {
            sd = client_socket;
            if (FD_ISSET(sd, &readfds)) {
                // 读取数据
                ssize_t valread = read(sd, &data, sizeof(data));
                if (valread == 0) {
                    // 客户端关闭连接
                    getpeername(sd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
                    std::cout << "客户端断开连接，ip:" << inet_ntoa(address.sin_addr) <<
                              ", port:" << ntohs(address.sin_port) << std::endl;
                    close(sd);
                    client_socket = 0;
                } else {
                    // 处理接收到的数据
                    pthread_mutex_lock(&data_mutex);
                    std::cout << "Received: " << data.id << ", " << data.value << ", " << data.name << std::endl;
                    pthread_mutex_unlock(&data_mutex);
                    pthread_create(&tid, nullptr, sendMsg, &data);
                    pthread_detach(tid);
                }
            }
        }
    }
}

void *TCPServer::sendMsg(void *arg) {
    int sock = 0;
    struct sockaddr_in serv_addr{};
    Data data{};

    // 复制数据以避免数据竞争
    data = *(Data *) arg;

    // 创建socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return nullptr;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SENDPORT);
    // 将IPv4地址从文本转换为二进制形式
    if (inet_pton(AF_INET, SENDIP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        close(sock);
        return nullptr;
    }

    // 连接到服务器
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        close(sock);
        return nullptr;
    }

    // 发送数据
    if (send(sock, &data, sizeof(data), 0) < 0) {
        std::cerr << "Send failed" << std::endl;
    }

    close(sock);
    return nullptr;
}
