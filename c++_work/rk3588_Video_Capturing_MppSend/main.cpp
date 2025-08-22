#include "PCIEDriver.h"
#include <chrono>
#include <thread>

int main() {
    PCIEDriver driver(1920 * 1080 * 16 / 8);
//    test_pcieDriver();
    driver.StartThread();

    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
