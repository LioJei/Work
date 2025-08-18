//
// Created by andrew on 2025/8/11.
//

#include "PCIEDriver.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <errno.h>
#include <thread>
#include <sys/ioctl.h>


PCIEDriver::PCIEDriver(unsigned int len)
        : m_len(len),
          PCIE_buf(nullptr),
          bram_base(nullptr),
          dma_fd(-1),
          bram_fd(-1),
          send_fd(-1),
          data_fp(nullptr) {
    if (m_len > PCIE_BUFFER_FRAME_SIZE) m_len = PCIE_BUFFER_FRAME_SIZE;

    if ((dma_fd = open(C2H_DEV, O_RDWR | O_NONBLOCK)) < 0) {
        perror("Open C2H device failed");
        exit(EXIT_FAILURE);
    }
    if ((bram_fd = open(USER_DEV, O_RDWR | O_NONBLOCK)) < 0) {
        perror("Open USER_DEV device failed");
        close(dma_fd);
        exit(EXIT_FAILURE);
    }
    if ((send_fd = open(H2C_DEV, O_RDWR | O_NONBLOCK)) < 0) {
        perror("Open C2H device failed");
        close(dma_fd);
        close(bram_fd);
        exit(EXIT_FAILURE);
    }
//    posix_memalign((void **) &PCIE_buf, 4096, BUFFER_COUNT * sizeof(PCIE_channel_buffer) + 4096);
    size_t map_size = sysconf(_SC_PAGESIZE);
    posix_memalign((void **) &PCIE_buf, map_size, PCIE_BUFFER_FRAME_SIZE + 4096);
    bram_base = mmap(nullptr, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, bram_fd, BRAM_BASE_ADDR);
    if (bram_base == MAP_FAILED) {
        perror("mmap bram_addr failed");
        close(dma_fd);
        close(bram_fd);
        close(send_fd);
        exit(EXIT_FAILURE);
    }
}

PCIEDriver::~PCIEDriver()
noexcept {
    if (dma_fd > 0)
        close(dma_fd);    // 关闭DMA设备文件描述符
    if (bram_fd > 0)
        close(bram_fd);    // 关闭BRAM设备文件描述符
    if (send_fd > 0)
        close(send_fd);    // 关闭发送设备文件描述符
    if (!PCIE_buf)
        free(PCIE_buf);    // 释放PCIe驱动缓冲块
    if (!bram_base)
        munmap(bram_base, 4096);    // 释放BRAM空间地址
    if (data_fp.is_open())
        data_fp.close();     // 关闭调试数据文件
}

void PCIEDriver::StartReceive() {
#if DEBUG == 2
    data_fp.open("testData.dat", std::ios::out | std::ios::binary | std::ios::trunc);
#endif
    int32_t buffer_id = 0;
    uint32_t read_reg = 0, frame_count = 0, frame = 0, sdi_x = 0, index = 0;
    while (true) {
        read_reg = PCIEReadBram(READ_SDI0_ADDR);
#if DEBUG >= 1
        printf("PCIE Read reg = %d\n", read_reg);
#endif
        frame = read_reg & 0x03;
        sdi_x = (read_reg >> 2) & 0x03;
#if DEBUG >= 1
        printf("PCIE Read frame = %d\n", frame);
        printf("PCIE Read sdi_x = %d\n", sdi_x);
#endif
        if (frame == 0) {
            frame = 3;
            PCIEWriteBram(frame, WRITE_SDI0_ADDR);
#if DEBUG >= 1
            printf("Linux Write frame = %d\n", frame);
#endif
        } else {

            PCIEWriteBram(--frame, WRITE_SDI0_ADDR);
#if DEBUG >= 1
            printf("Linux Write frame = %d\n", frame);
#endif
        }
        PCIEReadDMAFrame(PCIE_buf, frame * DATA_OFFSET_BITS);

#if DEBUG == 2
        data_fp.write((char *) PCIE_buf, PCIE_BUFFER_FRAME_SIZE);
#endif
#if DEBUG >= 1
        uint32_t data = *(uint32_t *) PCIE_buf;
        printf("PCIE Read frame_header = 0x%08x\n", data);
        printf("frame count: %d\n", frame_count++);
#endif
        buffer_id++;
        buffer_id %= BUFFER_COUNT;
    }
}

void PCIEDriver::StartThread() {
    std::thread recv_thread(&PCIEDriver::StartReceive, this);
    recv_thread.detach();
}

int PCIEDriver::ReadData(unsigned char *data, uint32_t addr) const {
    size_t rc = pread(bram_fd, data, m_len, addr);
    return (int) rc;
}

int PCIEDriver::PCIEReadDMA(unsigned char *data, unsigned int len, unsigned int addr) const {
    uint8_t size = pread(dma_fd, data, len, addr);
//    printf("PCIERead size = 0x%x\n", size);
    if (size < 0) {
        perror("PCIERead failed");
        return -1;
    }
//    printf("PCIERead size = 0x%x\n", PCIE_buf[0]);
    return 0;
}

void PCIEDriver::PCIEReadDMALine(unsigned char *data, unsigned int len, unsigned int addr) const {
    for (int i = 0; i < len; i++) {
        pread(dma_fd, data + i * PCIE_BUFFER_LINE_SIZE, PCIE_BUFFER_LINE_SIZE, addr + i * DATA_LINE_OFFSET);
    }
}

void PCIEDriver::PCIEReadDMAFrame(unsigned char *data, unsigned int addr) const {
//    memset(PCIE_buf, 0, 1080 * m_len);
//    for (int i = 0; i < 1080; i++) {
//        pread(dma_fd, data + i * PCIE_BUFFER_LINE_SIZE, PCIE_BUFFER_LINE_SIZE, addr + i * DATA_LINE_OFFSET);
//    }
    PCIEReadDMALine(data, 1080, addr);
}

int PCIEDriver::PCIEWriteDMA(unsigned char *data, unsigned int len, unsigned int addr) const {
    uint8_t size = pwrite(send_fd, data, len, addr);
    if (size < 0) return -1;
    return 0;
}

void PCIEDriver::PCIEWriteBram(uint32_t data, uint32_t offset) {
    volatile uint32_t *reg_ptr;
    reg_ptr = static_cast<volatile uint32_t *>(bram_base) + offset;
    *reg_ptr = (volatile uint32_t &) data;
}

uint32_t PCIEDriver::PCIEReadBram(uint32_t offset) {
    volatile uint32_t *reg_ptr;
    reg_ptr = static_cast<uint32_t *>(bram_base) + offset;
//    memcpy(static_cast<void *>(&data), static_cast<void *>(&reg_ptr), 4);
    return *reg_ptr;
}

void PCIEDriver::PCIEReadIO(const unsigned char *data, uint32_t addr) const {
    struct xdma_aperture_ioctl io{};

    io.buffer = (unsigned long) data;
    io.len = 4;
    io.ep_addr = addr;
    io.aperture = 4096;
    io.done = 0UL;

    int rc = ioctl(dma_fd, IOCTL_XDMA_APERTURE_R, &io);
    printf("PCIEReadIO rc = %d, error = %d\n", rc, io.error);
    if (rc < 0 || io.error) {
        fprintf(stderr,
                "aperture R failed %d,%d.\n", rc, io.error);
    }
}

void PCIEDriver::PCIEWriteIO(const unsigned char *data, uint32_t addr) const {
    struct xdma_aperture_ioctl io{};

    io.buffer = (unsigned long) data;
    io.len = 4;
    io.ep_addr = addr;
    io.aperture = 4096;
    io.done = 0UL;

    int rc = ioctl(send_fd, IOCTL_XDMA_APERTURE_W, &io);
    if (rc < 0 || io.error) {
        fprintf(stderr,
                "aperture W failed %d,%d.\n", rc, io.error);
    }
}

int PCIEDriver::CheckFrameHeader(const char *data, int datalen) {
    int pos = -1;

    if (!data)
        return pos;
    int offset = 0;
    while (offset < datalen - 4) {
        if (*(data + offset) == 0x55) {
            if (*(data + offset + 1) == 0xAA) {
                if (*(data + offset + 2) == 0xAA) {
                    if (*(data + offset + 3) == 0x55) {
                        pos = offset;
                        break;
                    } else {
                        offset++;
                    }
                } else {
                    offset++;
                }
            } else {
                offset++;
            }
        } else {
            offset++;
        }
    }

    return pos;
}