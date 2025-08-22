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
#include <sys/ioctl.h>
#include <sys/poll.h>


PCIEDriver::PCIEDriver(unsigned int len)
//        : m_len(len),
        : PCIE_buf(nullptr),
//          ring_buffer(BUFFER_COUNT),
          bram_base(nullptr),
          dma_fd(-1),
          bram_fd(-1),
          send_fd(-1),
          event_fd(-1),
          data_fp(nullptr),
          stopFlag(false),
          recv_data(nullptr),
          send_data(nullptr) {
//    if (m_len > PCIE_BUFFER_FRAME_SIZE) m_len = PCIE_BUFFER_FRAME_SIZE;

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
        perror("Open H2C device failed");
        close(dma_fd);
        close(bram_fd);
        exit(EXIT_FAILURE);
    }
    if ((event_fd = open(EVENT_DEV, O_RDWR | O_NONBLOCK, 0)) < 0) {
        perror("Open event device failed");
        close(dma_fd);
        close(bram_fd);
        close(send_fd);
        exit(EXIT_FAILURE);
    }
    size_t map_size = sysconf(_SC_PAGESIZE);
    posix_memalign((void **) &PCIE_buf, map_size, PCIE_BUFFER_FRAME_SIZE + 4096);
    posix_memalign((void **) &recv_data, map_size, PCIE_BUFFER_SIZE);
    posix_memalign((void **) &send_data, map_size, PCIE_BUFFER_SIZE);
    bram_base = mmap(nullptr, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, bram_fd, BRAM_BASE_ADDR);
    if (bram_base == MAP_FAILED) {
        perror("mmap bram_addr failed");
        close(dma_fd);
        close(bram_fd);
        close(send_fd);
        exit(EXIT_FAILURE);
    }
    data_fp.open("/run/media/mmcblk0p8/testData.yuv", std::ios::out | std::ios::binary | std::ios::trunc);
}

PCIEDriver::~PCIEDriver()
noexcept {
    if (dma_fd > 0)
        close(dma_fd);    // 关闭DMA设备文件描述符
    if (bram_fd > 0)
        close(bram_fd);    // 关闭BRAM设备文件描述符
    if (send_fd > 0)
        close(send_fd);    // 关闭发送设备文件描述符
    if (event_fd > 0)
        close(event_fd);    // 关闭中断设备文件描述符
    if (!PCIE_buf)
        free(PCIE_buf);    // 释放PCIe驱动缓冲块
    PCIE_buf = nullptr;
    if (!bram_base)
        munmap(bram_base, 4096);    // 释放BRAM空间地址
    if (data_fp.is_open())
        data_fp.close();     // 关闭调试数据文件
    if (recv_data) {
        free(recv_data);
        recv_data = nullptr;
    }
    if (send_data) {
        free(send_data);
        send_data = nullptr;
    }
    StopThread();    // 停止收数线程
}

void PCIEDriver::StartThread() {
    stopFlag = false;
    worker = std::thread(&PCIEDriver::StartReceive, this);
    send_worker = std::thread(&PCIEDriver::StartSend, this);
}

void PCIEDriver::StopThread() {
    stopFlag = true;
    if (worker.joinable()) {
        worker.join();
    }
    if (send_worker.joinable()) {
        send_worker.join();
    }
}


// 字节序转换函数
uint32_t PCIEDriver::swap_endian32(uint32_t value) {
    return ((value & 0x000000FF) << 24) |
           ((value & 0x0000FF00) << 8)  |
           ((value & 0x00FF0000) >> 8)  |
           ((value & 0xFF000000) >> 24);
}

// 转换整个数组
void PCIEDriver::convert_array_endian(uint8_t* data, size_t length) {
    size_t uint32_count = length / 4;

    for (size_t i = 0; i < uint32_count; i++) {
        uint32_t* word_ptr = (uint32_t*)(data + i * 4);
        *word_ptr = swap_endian32(*word_ptr);
    }
}


// 32位字块顺序反转
void PCIEDriver::reverse_32bit_word_order(uint8_t* data, size_t byte_length) {
    if (byte_length % 4 != 0) return;

    size_t word_count = byte_length / 4;
    uint32_t* words = (uint32_t*)data;

    for (size_t i = 0; i < word_count / 2; i++) {
        uint32_t temp = words[i];
        words[i] = words[word_count - 1 - i];
        words[word_count - 1 - i] = temp;
    }
}

// 同时进行字节序转换和字块顺序反转
void PCIEDriver::convert_endian_and_reverse_order(uint8_t* data, size_t byte_length) {
    if (byte_length % 4 != 0) return;

    size_t word_count = byte_length / 4;
    uint32_t* words = (uint32_t*)data;

    // 先进行字节序转换
    for (size_t i = 0; i < word_count; i++) {
        words[i] = swap_endian32(words[i]);
    }

    // 再反转字块顺序
    for (size_t i = 0; i < word_count;) {
        uint32_t temp1 = words[i];
        uint32_t temp2 = words[i + 1];
        uint32_t temp3 = words[i + 2];
        uint32_t temp4 = words[i + 3];
        words[i] = temp4;
        words[i + 1] = temp3;
        words[i + 2] = temp2;
        words[i + 3] = temp1;
        i = i + 4;
    }
}

// 只进行字节序转换
void PCIEDriver::reverse_bytes_in_words(uint8_t* data, size_t byte_length) {
    if (byte_length % 4 != 0) return;

    size_t word_count = byte_length / 4;
    uint32_t* words = (uint32_t*)data;

    for (size_t i = 0; i < word_count; i++) {
        words[i] = swap_endian32(words[i]);
    }
}

void PCIEDriver::DataSplit(uint8_t *data) {
    memset(PCIE_buf, 0, sizeof(PCIE_buf));
    for (int i = 0; i < 1080; i++) {
        memcpy(PCIE_buf + i * PCIE_BUFFER_LINE_SIZE, data + i * DATA_LINE_OFFSET, PCIE_BUFFER_LINE_SIZE);
    }
    convert_endian_and_reverse_order(PCIE_buf, PCIE_BUFFER_FRAME_SIZE);
#if DEBUG == 1
    for (int i = 0; i < 16; i++) {
        printf("%02x ", PCIE_buf[i]);
        if ((i + 1) % 3840 == 0) {
//            printf("\n");
        }
    }
#endif
}

void PCIEDriver::StartSend() {
    int index = 0;
#if DEBUG == 3
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
#endif
    while (!stopFlag) {
        memset(send_data, 0, PCIE_BUFFER_SIZE);
        if (buffer.pop(send_data)) {
            //TODO: 发送数据
#if DEBUG == 3
            start_time = std::chrono::high_resolution_clock::now();
#endif
            DataSplit(send_data);
#if DEBUG == 2
            if(index < 100) data_fp.write((char *) PCIE_buf, PCIE_BUFFER_FRAME_SIZE);
#endif
#if DEBUG == 3
            end_time = std::chrono::high_resolution_clock::now();
            std::cout << "Split time: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms\n";
#endif
#if DEBUG == 1
            printf("send:\n");
            for (int i = 0; i < PCIE_BUFFER_FRAME_SIZE; i++) {
                printf("%02x ", PCIE_buf[i]);
                if ((i + 1) % 3840 == 0) {
                    printf("\n");
                }
            }
#endif
        }
        index++;
    }
}

void PCIEDriver::StartReceive() {
    struct pollfd pfd = {event_fd, POLLIN, 0};
    int32_t count = 0;
    uint32_t read_reg = 0, frame = 0, sdi_x = 0, ret = 0;
#if DEBUG == 3
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
#endif
//    bool flag = false;
    while (!stopFlag) {
        ret = poll(&pfd, 1, -1);
        if (ret > 0 && (pfd.revents & POLLIN)) {
            uint32_t irq_status;
            read(event_fd, &irq_status, sizeof(irq_status));
            // 处理不同中断类型
#if DEBUG == 1
            printf("irq_status = %08x, count = %d\n", irq_status, ++count);
#endif
            read_reg = PCIEReadBram(READ_SDI0_ADDR);
#if DEBUG == 1
            printf("PCIE Read reg = %d\n", read_reg);
#endif
            frame = read_reg & 0x03;
            sdi_x = (read_reg >> 2) & 0x03;
#if DEBUG == 1
            printf("PCIE Read frame = %d\n", frame);
            printf("PCIE Read sdi_x = %d\n", sdi_x);
            printf("Linux Write frame = %d\n", frame);
#endif
            PCIEWriteBram(frame, WRITE_SDI0_ADDR);
#if DEBUG == 3
            start_time = std::chrono::high_resolution_clock::now();
#endif
            memset(recv_data, 0, PCIE_BUFFER_SIZE);
            PCIEReadPage(recv_data, frame * DATA_OFFSET_BITS);
#if DEBUG == 3
            end_time = std::chrono::high_resolution_clock::now();
            std::cout << "Read time: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms\n";
#endif
            buffer.push(recv_data);

#if DEBUG == 1
            printf("recv:");
            for (int i = 0; i < 16; i++) {
                printf("%02x ", recv_data[i]);
                if ((i + 1) % 16 == 0) {
                    printf("\n");
                }
            }
#endif
        } else if (ret == 0) {
            printf("poll timeout\n");
        }
    }
}

void PCIEDriver::PCIEReadPage(uint8_t *data, uint32_t addr) const {
    uint8_t size = pread(dma_fd, data, 4096 * 1080, addr);
    if (size < 0) {
        perror("PCIEReadPage failed");
    }
}

size_t PCIEDriver::ReadData(uint8_t *frame_data) {
    buffer.pop(frame_data);
    return 0;
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
    bool flag = false;
    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point end_time;
    for (int i = 0; i < len; i++) {
        if (!flag) {
            pread(dma_fd, data + i * PCIE_BUFFER_LINE_SIZE, PCIE_BUFFER_LINE_SIZE, addr + i * DATA_LINE_OFFSET);
            end_time = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() > 10) {
                printf("PCIEReadDMALine time out in lines %d\n", i);
                flag = true;
            }
        } else {
            memset(data + i * PCIE_BUFFER_LINE_SIZE, 0, PCIE_BUFFER_LINE_SIZE);
        }
    }
}

void PCIEDriver::PCIEReadDMAFrame(unsigned char *data, unsigned int addr) const {
//    memset(data, 0, PCIE_BUFFER_FRAME_SIZE);
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