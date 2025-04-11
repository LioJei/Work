#include "TcpServer.h"

int main(){
    TCPServer server(RECVIP, RECVPORT);
    server.start();

    return 0;
}
