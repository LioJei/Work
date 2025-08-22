#include "Display.h"
#include "PCIEDriver.h"
#include "UDPSocket.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <ctime>
#include <fcntl.h>
#include <sys/poll.h>

int get_time(int &hour, int &minute, int &second) {
    std::time_t now = std::time(nullptr);
    std::tm local_time{};

#if defined(_WIN32)
    // Windows使用localtime_s
    if (localtime_s(&local_time, &now) != 0) {
        std::cerr << "获取时间失败" << std::endl;
        return 1;
    }
#else
    // Linux和其他类Unix系统使用localtime_r
    if (localtime_r(&now, &local_time) == nullptr) {
        std::cerr << "获取时间失败" << std::endl;
        return 1;
    }
#endif

    hour = local_time.tm_hour;
    minute = local_time.tm_min;
    second = local_time.tm_sec;
}

void test_pcieDriver() {
    int hour, minute, second;
    get_time(hour, minute, second);
    PCIEDriver driver(1920 * 16 / 8);
    std::ofstream fp("test-" + std::to_string(hour) + "-" + std::to_string(minute) + "-" + std::to_string(second) + ".dat");
    std::ofstream fp2("test2.dat");

//    uint32_t a = 0x12345678,b=0x00000000;
//    driver.PCIEWriteBram(a, 0);
//    driver.PCIEReadBram(b,0);
//    std::cout << "b = " << std::hex << b << std::endl;
    unsigned char *send_data, *recv_data;
    posix_memalign((void**)&send_data, 4096, PCIE_BUFFER_LINE_SIZE);
//    auto *recv_data = (unsigned char*) malloc(PCIE_BUFFER_SIZE);
    posix_memalign((void**)&recv_data, 4096, PCIE_BUFFER_FRAME_SIZE);
    memset(send_data, 0xbb, 4);
    memset(recv_data, 0, 4);

//    driver.PCIEWriteIO(send_data, WRITE_SDI0_ADDR);
//    driver.PCIEReadIO(recv_data, WRITE_SDI0_ADDR);
    driver.PCIEReadDMAFrame(recv_data, DATA_SDI0_2_ADDR);
    fp.write((char*)recv_data, PCIE_BUFFER_FRAME_SIZE);
//    driver.PCIEReadDMAFrame(recv_data, DATA_SDI0_3_ADDR);
//    fp2.write((char*)recv_data, PCIE_BUFFER_SIZE * 1080);
//    driver.PCIEReadIO(recv_data, 0);
    printf("recv_data:\n");
    int count = 0;
    for(int i=0;i<4;i++){
        printf("%08x \n", recv_data[i]);
        count++;
    }
    if(count!= 4) {
        printf("error count.\n");
    }
    printf("count = %d\n", count);
    free(send_data);
    free(recv_data);
    fp.close();
    fp2.close();
}

int main() {
    PCIEDriver driver(1920 * 1080 * 16 / 8);
//    test_pcieDriver();
    driver.StartThread();

    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
