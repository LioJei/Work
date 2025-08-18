/**
 * @author <lijunjie>
 * @date 2025-08-11
 * @brief This file defines the interface of PCIEDriver.
 *
 * */

#ifndef RK3588_PROJ_PCIEDRIVER_H
#define RK3588_PROJ_PCIEDRIVER_H
///头文件包含
#include "BlockQueue.h"
#include "cdev_sgdma.h"
#include <fstream>
#include <sys/mman.h>
///调试宏
#define DEBUG               2 // 0:关闭调试信息, 1:开启调试信息, 2:开启调试信息输出到文件
/// 设备路径定义
#define USER_DEV            "/dev/xdma0_user"
#define H2C_DEV             "/dev/xdma0_h2c_0"
#define C2H_DEV             "/dev/xdma0_c2h_0"
/// FPGA地址定义
// BRAM空间地址
#define BRAM_BASE_ADDR      0x00000000  // BRAM在PCIE空间的基地址
#define WRITE_SDI0_ADDR     (BRAM_BASE_ADDR + 2) // SDI0写入地址
#define WRITE_SDI1_ADDR     (BRAM_BASE_ADDR + 3) // SDI1写入地址
#define READ_SDI0_ADDR      (BRAM_BASE_ADDR + 4)   // SDI0读取地址
#define READ_SDI1_ADDR      (BRAM_BASE_ADDR + 5)   // SDI1读取地址
// IO空间地址
#define DATA_OFFSET_BITS    0x800000  // 数据在PCIE空间的偏移量
#define DATA_BASE_ADDR      0x00000000  // 数据在PCIE空间的基地址
#define DATA_SDI0_1_ADDR    DATA_BASE_ADDR  // SDI0/1数据地址
#define DATA_SDI0_2_ADDR    (DATA_SDI0_1_ADDR + DATA_OFFSET_BITS)  // SDI0/2数据地址
#define DATA_SDI0_3_ADDR    (DATA_SDI0_2_ADDR + DATA_OFFSET_BITS)  // SDI0/3数据地址
#define DATA_SDI0_4_ADDR    (DATA_SDI0_3_ADDR + DATA_OFFSET_BITS)  // SDI0/4数据地址
#define DATA_SDI1_1_ADDR    (DATA_SDI0_4_ADDR + DATA_OFFSET_BITS)  // SDI1/1数据地址
#define DATA_SDI1_2_ADDR    (DATA_SDI1_1_ADDR + DATA_OFFSET_BITS)  // SDI1/2数据地址
#define DATA_SDI1_3_ADDR    (DATA_SDI1_2_ADDR + DATA_OFFSET_BITS)  // SDI1/3数据地址
#define DATA_SDI1_4_ADDR    (DATA_SDI1_3_ADDR + DATA_OFFSET_BITS)  // SDI1/4数据地址
#define DATA_LINE_OFFSET    0x1000  // 数据帧每行偏移量
//数据定义
#define WIDTH 1920
#define HEIGHT 1080
#define BYTES_PER_PIXEL 2
#define PCIE_BUFFER_LINE_SIZE    (WIDTH * BYTES_PER_PIXEL)  // 1080P分辨率下，一帧YUV数据一行占用空间大小
#define PCIE_BUFFER_FRAME_SIZE    (WIDTH * HEIGHT * BYTES_PER_PIXEL)  // 1080P分辨率下，一帧YUV数据一行占用空间大小
#define BUFFER_COUNT             2  // 缓存块数量
struct PCIE_channel_buffer {
    unsigned char buffer[PCIE_BUFFER_FRAME_SIZE];
    unsigned int length;
}__attribute__ ((aligned(4096)));

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
     * @brief:     PCIE数据读取
     * @param[in]: offet(数据偏移)
     * return:     读取数据指针
     * */
    int ReadData(unsigned char *data, uint32_t addr) const;
    /**
     * @brief:     DMA一帧一行数据读取
     * @param[in]: data(读取数据存储ptr)
     * @param[in]: len(读取数据行长度)
     * @param[in]: addr(读取地址)
     * return:     读取数据长度
     * */
    void PCIEReadDMALine(unsigned char *data, unsigned int len, uint32_t addr) const;

    /**
     * @brief:     DMA数据一帧读取
     * @param[in]: data(读取数据存储ptr)
     * @param[in]: len(读取数据长度)
     * @param[in]: addr(读取地址)
     * return:     读取数据长度
     * */
    void PCIEReadDMAFrame(unsigned char *data, uint32_t addr) const;

    /**
     * @brief:     DMA数据读取
     * @param[in]: data(读取数据存储ptr)
     * @param[in]: len(读取数据长度)
     * @param[in]: addr(读取地址)
     * @return:    -1:失败, 0:成功
     * */
    int PCIEReadDMA(unsigned char *data, unsigned int len, uint32_t addr) const;

    /**
     * @brief:     DMA数据写入
     * @param[in]: data(写入数据存储ptr)
     * @param[in]: len(写入数据长度)
     * @param[in]: addr(写入地址)
     * @return:     -1:失败, 0:成功
     * */
    int PCIEWriteDMA(unsigned char *data, unsigned int len, uint32_t addr) const;

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
     * @brief:     读取帧头
     * @param[in]: data(帧头数据存储ptr)
     * return:     帧头数据长度
     * */
    /**
     * @brief:     IOCTL指令读取
     * @param[in]: data(读取数据存储ptr)
     * @param[in]: addr(读取地址)
     * */
    void PCIEReadIO(const unsigned char *data, uint32_t addr) const;

    /**
     * @brief:     IOCTL指令写入
     * @param[in]: data(写入数据存储ptr)
     * @param[in]: addr(写入地址)
     * */
    void PCIEWriteIO(const unsigned char *data, uint32_t addr) const;
    static int CheckFrameHeader(const char *data, int dataLen);

    /**
     * @brief:     开启线程
     * */
    void StartThread();

private:
    /**
     * @brief 开启PCIE传输
     * */
    void StartReceive();

    unsigned int m_len;             // PCIE一次传输的数据长度
    unsigned char *PCIE_buf;          // PCIE驱动缓冲块
    void *bram_base;                // BRAM空间地址
    int dma_fd;                     // DMA设备文件描述符号
    int bram_fd;                    // BRAM设备文件描述符号
    int send_fd;                    // 发送设备文件描述符号
    std::ofstream data_fp;               // 调试数据文件
};


#endif //RK3588_PROJ_PCIEDRIVER_H
