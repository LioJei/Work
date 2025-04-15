#include "TcpServer.h"
#include "Logger.h"

int main(){
    auto logger = std::make_shared<Logger>(FILE_NAME, FILE_SIZE);
    TCPServer server(RECVIP, RECVPORT, logger);
    server.start();

    return 0;
}