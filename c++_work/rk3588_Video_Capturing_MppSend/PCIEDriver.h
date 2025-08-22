/**
 * @author <lijunjie>
 * @date 2025-08-11
 * @brief This file defines the interface of PCIEDriver.
 *
 * */

#ifndef RK3588_PROJ_PCIEDRIVER_H
#define RK3588_PROJ_PCIEDRIVER_H
///头文件包含
#include "RingBuffer.h"
#include <fstream>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <ratio>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
///     调试宏
#define DEBUG               2 // 0:关闭调试信息, 1:开启调试信息, 2:开启调试信息输出到文件, 3:延时测试
///     设备路径定义
const char *const USER_DEV = "/dev/xdma0_user";
const char *const C2H_DEV = "/dev/xdma0_c2h_0";
const char *const EVENT_DEV_0 = "/dev/xdma0_events_0";
const char *const EVENT_DEV_1 = "/dev/xdma0_events_1";

///FPGA地址定义
//      BRAM空间地址
constexpr size_t BRAM_BASE_ADDR = 0x00000000;  // BRAM在PCIE空间的基地址
constexpr size_t WRITE_SDI0_ADDR = BRAM_BASE_ADDR + 2; // SDI0写入地址
constexpr size_t WRITE_SDI1_ADDR = BRAM_BASE_ADDR + 3; // SDI1写入地址
constexpr size_t READ_SDI0_ADDR = BRAM_BASE_ADDR + 4;  // SDI0读取地址
constexpr size_t READ_SDI1_ADDR = BRAM_BASE_ADDR + 5;   // SDI1读取地址
//      DMA空间地址
constexpr size_t DATA_LINE_OFFSET = 0x1000;  // 数据帧每行偏移量
constexpr size_t DATA_OFFSET_BITS = 0x800000;  // 数据在PCIE空间的偏移量
constexpr size_t DATA_BASE_ADDR = 0x00000000;  // 数据在PCIE空间的基地址
constexpr size_t DATA_SDI0_1_ADDR = DATA_BASE_ADDR;  // SDI0/1数据地址
constexpr size_t DATA_SDI0_2_ADDR = DATA_SDI0_1_ADDR + DATA_OFFSET_BITS;  // SDI0/2数据地址
constexpr size_t DATA_SDI0_3_ADDR = DATA_SDI0_2_ADDR + DATA_OFFSET_BITS;  // SDI0/3数据地址
constexpr size_t DATA_SDI0_4_ADDR = DATA_SDI0_3_ADDR + DATA_OFFSET_BITS;  // SDI0/4数据地址
constexpr size_t DATA_SDI1_1_ADDR = DATA_SDI0_4_ADDR + DATA_OFFSET_BITS;  // SDI1/1数据地址
constexpr size_t DATA_SDI1_2_ADDR = DATA_SDI1_1_ADDR + DATA_OFFSET_BITS;  // SDI1/2数据地址
constexpr size_t DATA_SDI1_3_ADDR = DATA_SDI1_2_ADDR + DATA_OFFSET_BITS;  // SDI1/3数据地址
constexpr size_t DATA_SDI1_4_ADDR = DATA_SDI1_3_ADDR + DATA_OFFSET_BITS;  // SDI1/4数据地址
///数据定义
constexpr size_t WIDTH = 1920;  // 图像宽度
constexpr size_t HEIGHT = 1080;  // 图像高度
constexpr size_t BYTES_PER_PIXEL = 2;    // 像素字节数//数据定义
constexpr size_t PCIE_BUFFER_LINE_SIZE = WIDTH * BYTES_PER_PIXEL;  // 1080P分辨率下，一帧YUV数据一行占用空间大小
constexpr size_t PCIE_BUFFER_FRAME_SIZE = WIDTH * HEIGHT * BYTES_PER_PIXEL;  // 1080P分辨率下，一帧YUV数据一行占用空间大小
constexpr size_t PCIE_BUFFER_SIZE = 4096 * HEIGHT;  // pcie缓存区大小
constexpr size_t BUFFER_COUNT = 10;  // 缓存块数量;

class PCIEDriver {
public:
    /**
     * @brief:     构造函数
     * @param[in]: len(PCIE一次读取的数据长度)
     * */
    explicit PCIEDriver(unsigned int len);

    /**
     * @brief:     析构函数,释放资源
     * */
    ~PCIEDriver() noexcept;

    /**
     * @brief:     读取一页数据
     * @param[in]: data(读取数据存储ptr)
     * @param[in]: addr(读取地址)
     * */
    void PCIEReadPage(uint8_t *data, uint32_t addr) const;

    /**
     * @brief:     数据分割
     * @param[in]: data(需要分割的数据存储ptr)
     * @param[in]: PCIE_buf(PCIE驱动缓冲区)
     * */
    void DataSplit(uint8_t *data, uint8_t *PCIE_buf);

    /**
     * @brief:     BRAM数据写入
     * @param[in]: data(写入数据)
     * @param[in]: offset(写入地址)
     * */
    void PCIEWriteBram(uint32_t data, uint32_t offset);

    /**
     * @brief:     BRAM数据读取
     * @param[in]: offset(读取地址)
     * @retval:     uint32_t(读取数据)
     * */
    uint32_t PCIEReadBram(uint32_t offset);

    /**
     * @brief:     开启任务线程
     * */
    void StartThread();

    /**
     * @brief:     停止线程
     */
    void StopThread();

    /**
     * @brief 开启SDI0 PCIE传输
     * */
    void StartReceive0();

    /**
     * @brief 开启SDI1 PCIE传输
     * */
    void StartReceive1();

    /**
     * @brief 开启SDI0YUV数据传输
     * */
    void StartSend0();

    /**
    * @brief 开启SDI0YUV数据传输
    * */
    void StartSend1();

    /**
     * @brief:     32位字节序转换
     * @param[in]: value(需要转换的数据)
     * @retval:     uint32_t(转换后的数据)
     * */
    uint32_t Swap32(uint32_t value);

    /**
     * @brief:     128位字节序转换
     * @param[in]: value(需要转换的数据)
     * @param[in]: byte_length(转换字节数)
     * */
    void Swap128(uint8_t *data, size_t byte_length);

private:

    void *bram_base;                // BRAM空间地址
    int dma_fd;                     // DMA设备文件描述符号
    int bram_fd;                    // BRAM设备文件描述符号
    int event_fd0;                  // SDI0事件设备文件描述符号
    int event_fd1;                  // SDI1事件设备文件描述符号
    std::ofstream data_fp0;         // SDI0调试数据文件
    std::ofstream data_fp1;         // SDI1调试数据文件
    uint8_t *PCIE_buf0;             // SDI0 PCIE驱动缓冲块
    uint8_t *PCIE_buf1;             // SDI1 PCIE驱动缓冲块
    std::thread recv_worker0;       //SDI0收数线程句柄
    std::thread recv_worker1;       //SDI1收数线程句柄
    std::thread send_worker0;       //SDI0发送线程句柄
    std::thread send_worker1;       //SDI1发送线程句柄
    std::atomic<bool> stopFlag;     //线程停止标志
    FrameRingBuffer buffer0;        // SDI0缓存区
    FrameRingBuffer buffer1;        // SDI1缓存区
    uint8_t *recv_data0;            // SDI0收数数据
    uint8_t *recv_data1;            // SDI1收数数据
    uint8_t *send_data0;            // SDI0发送数据
    uint8_t *send_data1;            // SDI1发送数据
};


#endif //RK3588_PROJ_PCIEDRIVER_H
