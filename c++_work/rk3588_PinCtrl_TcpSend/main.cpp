#include "TcpServer.h"
#include "Logger.h"

int main(){
    try {
        auto logger = std::make_shared<Logger>(FILE_NAME, FILE_SIZE);
        TCPServer server(RECVIP, RECVPORT, logger);
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "程序运行时发生错误: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "程序运行时发生未知错误" << std::endl;
        return -2;
    }

    return 0;
}