///**
// * @author <lijunjie>
// * @date 2025-08-12
// * @brief This file defines the interface of UDP socket.
// *
// * */
//
//#ifndef RK3588_PROJ_UDPSOCKET_H
//#define RK3588_PROJ_UDPSOCKET_H
////头文件包含
//#include <cstdint>
//#include <vector>
//#include <stdexcept>
//#include <iostream>
//#include <vector>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <unistd.h>
//#include <cerrno>
//#include <cstring>
//#include <chrono>
//#include <thread>
//
////宏定义
//const char *SERVER_IP = "127.0.0.1";
//const int SERVER_PORT = 8888;
//const int MAX_RETRIES = 5;
//const int TIMEOUT_MS = 1000; // 1秒超时
//// 包头定义
//#pragma pack(push, 1) // 确保1字节对齐
//
//struct PacketHeader {
//    uint16_t magic;     // 魔数 0xAA55
//    uint8_t version;    // 协议版本
//    uint8_t type;       // 包类型
//    uint32_t sequence;  // 序列号
//    uint16_t length;    // 数据长度
//    uint16_t checksum;  // CRC16校验和
//
//    // 序列化为字节数组
//    void serialize(uint8_t *buffer) const {
//        uint16_t net_magic = htons(magic);
//        uint32_t net_sequence = htonl(sequence);
//        uint16_t net_length = htons(length);
//        uint16_t net_checksum = htons(checksum);
//
//        memcpy(buffer, &net_magic, 2);
//        memcpy(buffer + 2, &version, 1);
//        memcpy(buffer + 3, &type, 1);
//        memcpy(buffer + 4, &net_sequence, 4);
//        memcpy(buffer + 8, &net_length, 2);
//        memcpy(buffer + 10, &net_checksum, 2);
//    }
//
//    // 从字节数组反序列化
//    void deserialize(const uint8_t *buffer) {
//        memcpy(&magic, buffer, 2);
//        magic = ntohs(magic);
//
//        version = buffer[2];
//        type = buffer[3];
//
//        memcpy(&sequence, buffer + 4, 4);
//        sequence = ntohl(sequence);
//
//        memcpy(&length, buffer + 8, 2);
//        length = ntohs(length);
//
//        memcpy(&checksum, buffer + 10, 2);
//        checksum = ntohs(checksum);
//    }
//
//    // 计算包头CRC16校验
//    uint16_t calculateChecksum() const {
//        // 简单校验和示例 (实际应使用更复杂的CRC算法)
//        uint16_t sum = 0;
//        const uint8_t *data = reinterpret_cast<const uint8_t *>(this);
//        for (int i = 0; i < 10; i++) { // 前10字节参与校验
//            sum = (sum + data[i]) % 0xFFFF;
//        }
//        return sum;
//    }
//
//    // 验证包头有效性
//    bool isValid() const {
//        return magic == 0xAA55 &&
//               version == 0x01 &&
//               checksum == calculateChecksum();
//    }
//};
//
//#pragma pack(pop) // 恢复默认对齐
//// 协议常量定义
//const uint16_t MAX_PACKET_SIZE = 1400; // 最大包大小 (考虑MTU)
//const uint16_t HEADER_SIZE = sizeof(PacketHeader);
//const uint16_t MAX_PAYLOAD_SIZE = MAX_PACKET_SIZE - HEADER_SIZE;
//// 包构建器
//class PacketBuilder {
//public:
//    // 创建数据包
//    static std::vector <uint8_t> createDataPacket(
//            uint32_t sequence,
//            const uint8_t *data,
//            uint16_t length
//    ) {
//        if (length > MAX_PAYLOAD_SIZE) {
//            throw std::runtime_error("Payload too large");
//        }
//
//        PacketHeader header;
//        header.magic = 0xAA55;
//        header.version = 0x01;
//        header.type = 0x01; // 数据包
//        header.sequence = sequence;
//        header.length = length;
//        header.checksum = 0; // 先置0
//
//        // 计算校验和
//        header.checksum = header.calculateChecksum();
//
//        // 构建完整包
//        std::vector <uint8_t> packet(HEADER_SIZE + length);
//        header.serialize(packet.data());
//
//        // 添加数据
//        if (length > 0) {
//            memcpy(packet.data() + HEADER_SIZE, data, length);
//        }
//
//        return packet;
//    }
//
//    // 创建ACK包
//    static std::vector <uint8_t> createAckPacket(uint32_t sequence) {
//        PacketHeader header;
//        header.magic = 0xAA55;
//        header.version = 0x01;
//        header.type = 0x02; // ACK包
//        header.sequence = sequence;
//        header.length = 0;
//        header.checksum = 0;
//
//        header.checksum = header.calculateChecksum();
//
//        std::vector <uint8_t> packet(HEADER_SIZE);
//        header.serialize(packet.data());
//
//        return packet;
//    }
//
//    // 创建结束包
//    static std::vector <uint8_t> createEndPacket() {
//        PacketHeader header;
//        header.magic = 0xAA55;
//        header.version = 0x01;
//        header.type = 0x03; // 结束包
//        header.sequence = 0;
//        header.length = 0;
//        header.checksum = 0;
//
//        header.checksum = header.calculateChecksum();
//
//        std::vector <uint8_t> packet(HEADER_SIZE);
//        header.serialize(packet.data());
//
//        return packet;
//    }
//
//    // 解析接收到的包
//    static bool parsePacket(
//            const uint8_t *data,
//            size_t size,
//            PacketHeader &header,
//            const uint8_t *&payload
//    ) {
//        if (size < HEADER_SIZE) {
//            return false; // 包太小
//        }
//
//        header.deserialize(data);
//        if (!header.isValid()) {
//            return false; // 无效包头
//        }
//
//        if (size < HEADER_SIZE + header.length) {
//            return false; // 数据长度不匹配
//        }
//
//        payload = (header.length > 0) ? data + HEADER_SIZE : nullptr;
//        return true;
//    }
//};
//
//class UDPSocket {
//public:
//    /**
//     * @brief 构造函数
//     * @details 创建一个UDP套接字并连接到服务器
//     * */
//    UDPSocket();
//    /**
//     * @brief 析构函数
//     * @details 关闭套接字并释放资源
//     * */
//    ~UDPSocket();
//    /**
//     * @brief 发送数据包
//     * @details 外部接口，会调用内部的sendWithRetry函数
//     * @param data 待发送的数据包
//     * */
//    void sendData(const std::vector <uint8_t> &data);
//
//private:
//    /**
//     * @brief 发送数据包
//     * @details 发送数据包并重试若干次
//     * @param packet 待发送的数据包
//     * */
//    bool sendWithRetry(const std::vector <uint8_t> &packet);
//
//    int sockfd;                 // 套接字文件描述符
//    struct sockaddr_in server_addr;  // 服务器地址
//    uint32_t sequence;          // 序列号
//};
//
//
//#endif //RK3588_PROJ_UDPSOCKET_H
