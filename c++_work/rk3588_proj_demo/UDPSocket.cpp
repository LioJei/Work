////
//// Created by Administrator on 2025/8/12.
////
//
//#include "UDPSocket.h"
//
//UDPSocket::UDPSocket() : sequence(1) {
//    // 创建UDP套接字
//    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//    if (sockfd < 0) {
//        throw std::runtime_error("Socket creation failed");
//    }
//
//    // 配置服务器地址
//    memset(&server_addr, 0, sizeof(server_addr));
//    server_addr.sin_family = AF_INET;
//    server_addr.sin_port = htons(SERVER_PORT);
//    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
//        close(sockfd);
//        throw std::runtime_error("Invalid address");
//    }
//
//    // 设置超时
//    struct timeval tv;
//    tv.tv_sec = TIMEOUT_MS / 1000;
//    tv.tv_usec = (TIMEOUT_MS % 1000) * 1000;
//    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
//}
//
//UDPSocket::~UDPSocket() {
//    close(sockfd);
//}
//
//void UDPSocket::sendData(const std::vector <uint8_t> &data) {
//    size_t offset = 0;
//
//    while (offset < data.size()) {
//        // 计算本次发送的数据量
//        size_t chunk_size = std::min(
//                static_cast<size_t>(MAX_PAYLOAD_SIZE),
//                data.size() - offset
//        );
//
//        // 创建数据包
//        auto packet = PacketBuilder::createDataPacket(
//                sequence,
//                data.data() + offset,
//                chunk_size
//        );
//
//        // 发送并等待确认
//        if (!sendWithRetry(packet)) {
//            std::cerr << "Failed to send packet #" << sequence
//                      << " after " << MAX_RETRIES << " retries" << std::endl;
//            break;
//        }
//
//        offset += chunk_size;
//        sequence++;
//    }
//
//    // 发送结束包
//    auto end_packet = PacketBuilder::createEndPacket();
//    sendto(sockfd, end_packet.data(), end_packet.size(), 0,
//           (struct sockaddr *) &server_addr, sizeof(server_addr));
//}
//
//bool UDPSocket::sendWithRetry(const std::vector <uint8_t> &packet) {
//    for (int attempt = 0; attempt < MAX_RETRIES; attempt++) {
//        // 发送数据包
//        if (sendto(sockfd, packet.data(), packet.size(), 0,
//                   (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
//            std::cerr << "Send error: " << strerror(errno) << std::endl;
//            continue;
//        }
//
//        // 等待ACK
//        uint8_t buffer[MAX_PACKET_SIZE];
//        socklen_t addr_len = sizeof(server_addr);
//        ssize_t recv_bytes = recvfrom(sockfd, buffer, sizeof(buffer), 0,
//                                      (struct sockaddr *) &server_addr, &addr_len);
//
//        if (recv_bytes > 0) {
//            PacketHeader header;
//            const uint8_t *payload;
//
//            if (PacketBuilder::parsePacket(buffer, recv_bytes, header, payload) &&
//                header.type == 0x02 && // ACK包
//                header.sequence == sequence) {
//                return true; // 收到正确ACK
//            }
//        }
//
//        // 重试前等待
//        std::this_thread::sleep_for(std::chrono::milliseconds(200));
//    }
//
//    return false; // 所有重试失败
//}