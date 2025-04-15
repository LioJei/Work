#include "TcpServer.h"

#include <utility>

std::string floatToString(float value) {
    std::string str = std::to_string(value);

    // 去掉末尾的零
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);

    // 去掉末尾的点（如果有）
    if (str.back() == '.') {
        str.pop_back();
    }
    return str;
}

TCPServer::TCPServer(const char *ip, int port, std::shared_ptr<Logger> logger):
m_logger(std::move(logger))
{
    opt = 1;
    addrlen = sizeof(address);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        LOG(m_logger, Logger::ERROR, "socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        LOG(m_logger, Logger::ERROR, "setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        LOG(m_logger, Logger::ERROR, "bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        LOG(m_logger, Logger::ERROR, "listen error");
        exit(EXIT_FAILURE);
    }
    LOG(m_logger, Logger::INFO, "Server Starting...");
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
            LOG(m_logger, Logger::ERROR, "select error");
            exit(EXIT_FAILURE);
        }

        // 如果有新的连接请求
        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *) &address,
                                     (socklen_t *) &addrlen)) < 0) {
                LOG(m_logger, Logger::ERROR, "accept error");
                exit(EXIT_FAILURE);
            }
            LOG(m_logger, Logger::INFO, "New client has connected, client socket fd is:" + std::to_string(new_socket));
            // 将新的socket添加到客户端数组中
            for (int &client_socket: client_sockets) {
                if (client_socket == 0) {
                    client_socket = new_socket;
                    LOG(m_logger, Logger::INFO, "Client has added, client socket fd is:" + std::to_string(new_socket));
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
                    LOG(m_logger, Logger::INFO,
                        "Client has disconnected,IP:" + std::string(inet_ntoa(address.sin_addr)) +
                        ",PORT:" + std::to_string(ntohs(address.sin_port)));
                    close(sd);
                    client_socket = 0;
                } else {
                    // 处理接收到的数据
                    pthread_mutex_lock(&data_mutex);
                    LOG(m_logger, Logger::INFO, "Received:" + std::to_string(data.id) + "," + floatToString(data.value)
                                                + "," + std::string(data.name));
                    pthread_mutex_unlock(&data_mutex);
                    pthread_create(&tid, nullptr, [](void* arg) -> void* {
                        return static_cast<TCPServer*>(arg)->sendMsg(arg);}, &data);
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
        LOG(m_logger, Logger::ERROR, "Socket creation error");
        return nullptr;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SENDPORT);
    // 将IPv4地址从文本转换为二进制形式
    if (inet_pton(AF_INET, SENDIP, &serv_addr.sin_addr) <= 0) {
        LOG(m_logger, Logger::ERROR, "Invalid address/ Address not supported");
        close(sock);
        return nullptr;
    }

    // 连接到服务器
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        LOG(m_logger, Logger::ERROR, "Connection Failed");
        close(sock);
        return nullptr;
    }

    // 发送数据
    if (send(sock, &data, sizeof(data), 0) < 0) {
        LOG(m_logger, Logger::ERROR, "Send failed");
    }

    close(sock);
    return nullptr;
}
