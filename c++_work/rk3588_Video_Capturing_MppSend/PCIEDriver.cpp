//
// Created by andrew on 2025/8/11.
//

#include "PCIEDriver.h"

PCIEDriver::PCIEDriver(unsigned int len)
        : bram_base(nullptr),
          dma_fd(-1),
          bram_fd(-1),
          event_fd0(-1),
          event_fd1(-1),
          data_fp0(nullptr),
          data_fp1(nullptr),
          PCIE_buf0(nullptr),
          PCIE_buf1(nullptr),
          stopFlag(false),
          recv_data0(nullptr),
          recv_data1(nullptr),
          send_data0(nullptr),
          send_data1(nullptr) {
    if ((dma_fd = open(C2H_DEV, O_RDWR | O_NONBLOCK)) < 0) {
        perror("Open C2H device failed");
        exit(EXIT_FAILURE);
    }
    if ((bram_fd = open(USER_DEV, O_RDWR | O_NONBLOCK)) < 0) {
        perror("Open USER_DEV device failed");
        close(dma_fd);
        exit(EXIT_FAILURE);
    }
    if ((event_fd0 = open(EVENT_DEV_0, O_RDWR | O_NONBLOCK, 0)) < 0) {
        perror("Open event0 device failed");
        close(dma_fd);
        close(bram_fd);
        exit(EXIT_FAILURE);
    }
    if ((event_fd1 = open(EVENT_DEV_1, O_RDWR | O_NONBLOCK, 0)) < 0) {
        perror("Open event1 device failed");
        close(dma_fd);
        close(bram_fd);
        close(event_fd0);
        exit(EXIT_FAILURE);
    }
    size_t map_size = sysconf(_SC_PAGESIZE);
    posix_memalign((void **) &PCIE_buf0, map_size, PCIE_BUFFER_FRAME_SIZE + 4096);
    posix_memalign((void **) &PCIE_buf1, map_size, PCIE_BUFFER_FRAME_SIZE + 4096);
    posix_memalign((void **) &recv_data0, map_size, PCIE_BUFFER_SIZE);
    posix_memalign((void **) &recv_data1, map_size, PCIE_BUFFER_SIZE);
    posix_memalign((void **) &send_data0, map_size, PCIE_BUFFER_SIZE);
    posix_memalign((void **) &send_data1, map_size, PCIE_BUFFER_SIZE);
    bram_base = mmap(nullptr, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, bram_fd, BRAM_BASE_ADDR);
    if (bram_base == MAP_FAILED) {
        perror("mmap bram_addr failed");
        close(dma_fd);
        close(bram_fd);
        close(event_fd0);
        close(event_fd1);
        exit(EXIT_FAILURE);
    }
    data_fp0.open("/run/media/mmcblk0p8/SDI0_data.yuv", std::ios::out | std::ios::binary | std::ios::trunc);
    if (!data_fp0.is_open()) {
        perror("Open SDI0_data.yuv failed");
        close(dma_fd);
        close(bram_fd);
        close(event_fd0);
        close(event_fd1);
        munmap(bram_base, map_size);
        exit(EXIT_FAILURE);
    }
    data_fp1.open("/run/media/mmcblk0p8/SDI1_data.yuv", std::ios::out | std::ios::binary | std::ios::trunc);
    if (!data_fp1.is_open()) {
        perror("Open SDI0_data.yuv failed");
        close(dma_fd);
        close(bram_fd);
        close(event_fd0);
        close(event_fd1);
        munmap(bram_base, map_size);
        data_fp0.close();
        exit(EXIT_FAILURE);
    }
}

PCIEDriver::~PCIEDriver()
noexcept {
    if (dma_fd > 0)
        close(dma_fd);    // 关闭DMA设备文件描述符
    if (bram_fd > 0)
        close(bram_fd);    // 关闭BRAM设备文件描述符
    if (event_fd0 > 0)
        close(event_fd0);    // 关闭SDI0中断设备文件描述符
    if (event_fd1 > 0)
        close(event_fd1);    // 关闭SDI1中断设备文件描述符
    if (!PCIE_buf0){
        free(PCIE_buf0);    // 释放PCIe驱动缓冲块
        PCIE_buf0 = nullptr;
    }
    if (!PCIE_buf1){
        free(PCIE_buf1);    // 释放PCIe驱动缓冲块
        PCIE_buf1 = nullptr;
    }
    if (!bram_base)
        munmap(bram_base, 4096);    // 释放BRAM空间地址
    if (data_fp0.is_open())
        data_fp0.close();     // 关闭SDI0调试数据文件
    if (data_fp1.is_open())
        data_fp1.close();     // 关闭SDI1调试数据文件
    if (recv_data0) {
        free(recv_data0);
        recv_data0 = nullptr;
    }
    if (recv_data1) {
        free(recv_data1);
        recv_data1 = nullptr;
    }
    if (send_data0) {
        free(send_data0);
        send_data0 = nullptr;
    }
    if (send_data1) {
        free(send_data1);
        send_data1 = nullptr;
    }
    StopThread();    // 停止收数线程
}

void PCIEDriver::StartThread() {
    stopFlag = false;
    recv_worker0 = std::thread(&PCIEDriver::StartReceive0, this);
    recv_worker1 = std::thread(&PCIEDriver::StartReceive1, this);
    send_worker0 = std::thread(&PCIEDriver::StartSend0, this);
    send_worker1 = std::thread(&PCIEDriver::StartSend1, this);
}

void PCIEDriver::StopThread() {
    stopFlag = true;
    if (recv_worker0.joinable()) {
        recv_worker0.join();
    }
    if (recv_worker1.joinable()) {
        recv_worker1.join();
    }
    if (send_worker0.joinable()) {
        send_worker0.join();
    }
    if (send_worker1.joinable()) {
        send_worker1.join();
    }
}

uint32_t PCIEDriver::Swap32(uint32_t value) {
    return ((value & 0x000000FF) << 24) |
           ((value & 0x0000FF00) << 8) |
           ((value & 0x00FF0000) >> 8) |
           ((value & 0xFF000000) >> 24);
}

void PCIEDriver::Swap128(uint8_t *data, size_t byte_length) {
    if (byte_length % 4 != 0) return;

    size_t word_count = byte_length / 4;
    uint32_t *words = (uint32_t *) data;

    // 先进行字节序转换
    for (size_t i = 0; i < word_count; i++) {
        words[i] = Swap32(words[i]);
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

#if 1
static inline uint32_t Swap32X(uint32_t value)
{
    return ((value & 0x000000FF) << 24) |
           ((value & 0x0000FF00) << 8) |
           ((value & 0x00FF0000) >> 8) |
           ((value & 0xFF000000) >> 24);
}

static void inline Swap128X(uint8_t* pData, uint32_t iByteSize)
{
    if (pData == NULL || iByteSize == 0)
    {
        return ;
    }
    if (iByteSize % 4 != 0)
    {
        return;
    }

    uint32_t iWordSize = iByteSize / 4;
    uint32_t *pWData = (uint32_t *) pData;

    uint32_t iByte1 = 0;
    uint32_t iByte2 = 0;
    uint32_t iByte3 = 0;
    uint32_t iByte4 = 0;
#if 0
    // 先进行字节序转换
    for (size_t i = 0; i < iWordSize; i++)
    {
        pWData[i] = Swap32X(pWData[i]);
    }

    // 再反转字块顺序
    for (uint32_t i = 0; i < iWordSize; i+= 4)
    {
        iByte1 = pWData[i];
        iByte2 = pWData[i + 1];
        iByte3 = pWData[i + 2];
        iByte4 = pWData[i + 3];
        pWData[i]     = iByte4;
        pWData[i + 1] = iByte3;
        pWData[i + 2] = iByte2;
        pWData[i + 3] = iByte1;
    }
#else
    // 再反转字块顺序
    for (uint32_t i = 0; i < iWordSize; i+= 4)
    {
        iByte1 = Swap32X(pWData[i]);
        iByte2 = Swap32X(pWData[i + 1]);
        iByte3 = Swap32X(pWData[i + 2]);
        iByte4 = Swap32X(pWData[i + 3]);

        pWData[i]     = iByte4;
        pWData[i + 1] = iByte3;
        pWData[i + 2] = iByte2;
        pWData[i + 3] = iByte1;
    }
#endif

}
#else

#define Swap32X(value)                  \
    ((((value) & 0x000000FF) << 24)|   \
     (((value) & 0x0000FF00) << 8) |   \
     (((value) & 0x00FF0000) >> 8) |   \
     (((value) & 0xFF000000) >> 24))


#define Swap128X(pData, iBSize)                          \
do                                                      \
{                                                       \
    if ((pData) && (iBSize) && (((iBSize) % 4) == 0))   \
    {                                                   \
        uint32_t iWordSize = (iBSize) / 4;              \
        uint32_t *pWData = (uint32_t*)(pData);          \
                                                        \
        uint32_t iB1 = 0, iB2 = 0, iB3 = 0, iB4 = 0;    \
        for (uint32_t i = 0; i < iWordSize; i+= 4)      \
        {                                               \
            iB1 = Swap32X(pWData[i]);                    \
            iB2 = Swap32X(pWData[i + 1]);                \
            iB3 = Swap32X(pWData[i + 2]);                \
            iB4 = Swap32X(pWData[i + 3]);                \
            pWData[i]     = iB4;                        \
            pWData[i + 1] = iB3;                        \
            pWData[i + 2] = iB2;                        \
            pWData[i + 3] = iB1;                        \
        }                                               \
    }                                                   \
}while(0)

#endif

void PCIEDriver::DataSplit(uint8_t *data, uint8_t *PCIE_buf) {
    if (data == nullptr || PCIE_buf == nullptr) {
        perror("DataSplit failed, parameter is null");
        return;
    }
    memset(PCIE_buf, 0, sizeof(PCIE_buf));
    for (int i = 0; i < 1080; i++) {
        memcpy(PCIE_buf + i * PCIE_BUFFER_LINE_SIZE, data + i * DATA_LINE_OFFSET, PCIE_BUFFER_LINE_SIZE);
    }
    Swap128X(PCIE_buf, PCIE_BUFFER_FRAME_SIZE);
#if DEBUG == 1
    for (int i = 0; i < 16; i++) {
        printf("%02x ", PCIE_buf[i]);
        if ((i + 1) % 3840 == 0) {
//            printf("\n");
        }
    }
#endif
}

void PCIEDriver::StartSend0() {
    int index = 0;
#if DEBUG == 3
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
#endif
    while (!stopFlag) {
        memset(send_data0, 0, PCIE_BUFFER_SIZE);
        if (buffer0.pop(send_data0)) {
            printf("SDI0: %d\n", index);
#if DEBUG == 3
            start_time = std::chrono::high_resolution_clock::now();
#endif
            DataSplit(send_data0, PCIE_buf0);
#if DEBUG >= 2
            if (index < FP_WRITE_SIZE) data_fp0.write((char *) PCIE_buf0, PCIE_BUFFER_FRAME_SIZE);
            index++;
#endif
#if DEBUG == 3
            end_time = std::chrono::high_resolution_clock::now();
            std::cout << "Split time: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms\n";
#endif
            //TODO: 发送数据
        }
    }
}

void PCIEDriver::StartSend1() {
    int index = 0;
#if DEBUG == 3
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
#endif
    while (!stopFlag) {
        memset(send_data1, 0, PCIE_BUFFER_SIZE);
        if (buffer1.pop(send_data1)) {
            printf("SDI1: %d\n", index);
#if DEBUG == 3
            start_time = std::chrono::high_resolution_clock::now();
#endif
            DataSplit(send_data1, PCIE_buf1);
#if DEBUG >= 2
            if (index < FP_WRITE_SIZE) data_fp1.write((char *) PCIE_buf1, PCIE_BUFFER_FRAME_SIZE);
            index++;
#endif
#if DEBUG == 3
            end_time = std::chrono::high_resolution_clock::now();
            std::cout << "Split time: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms\n";
#endif
            //TODO: 发送数据
        }
    }
}

void PCIEDriver::StartReceive0() {
    struct pollfd pfd = {event_fd0, POLLIN, 0};
    int32_t count = 0;
    uint32_t read_reg = 0, frame = 0, ret = 0;
#if DEBUG == 3
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
#endif
//    bool flag = false;
    while (!stopFlag) {
        ret = poll(&pfd, 1, -1);
        if (ret > 0 && (pfd.revents & POLLIN)) {
            uint32_t irq_status;
            read(event_fd0, &irq_status, sizeof(irq_status));
            // 处理不同中断类型
            read_reg = PCIEReadBram(READ_SDI0_ADDR);
            frame = read_reg & 0x03;
            PCIEWriteBram(frame, WRITE_SDI0_ADDR);
#if DEBUG == 3
            start_time = std::chrono::high_resolution_clock::now();
#endif
            memset(recv_data0, 0, PCIE_BUFFER_SIZE);
            PCIEReadPage(recv_data0, DATA_SDI0_1_ADDR + frame * DATA_OFFSET_BITS);
#if DEBUG == 3
            end_time = std::chrono::high_resolution_clock::now();
            std::cout << "Read time: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms\n";
#endif
            buffer0.push(recv_data0);
        } else if (ret == 0) {
            printf("poll timeout\n");
        }
    }
}

void PCIEDriver::StartReceive1() {
    struct pollfd pfd = {event_fd1, POLLIN, 0};
    uint32_t read_reg = 0, frame = 0, ret = 0;
#if DEBUG == 3
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
#endif
//    bool flag = false;
    while (!stopFlag) {
        ret = poll(&pfd, 1, -1);
        if (ret > 0 && (pfd.revents & POLLIN)) {
            uint32_t irq_status;
            read(event_fd1, &irq_status, sizeof(irq_status));
            // 处理不同中断类型
            read_reg = PCIEReadBram(READ_SDI1_ADDR);
            frame = read_reg & 0x03;
            PCIEWriteBram(frame, WRITE_SDI1_ADDR);
#if DEBUG == 3
            start_time = std::chrono::high_resolution_clock::now();
#endif
            memset(recv_data1, 0, PCIE_BUFFER_SIZE);
            PCIEReadPage(recv_data1, DATA_SDI1_1_ADDR + frame * DATA_OFFSET_BITS);
#if DEBUG == 3
            end_time = std::chrono::high_resolution_clock::now();
            std::cout << "Read time: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms\n";
#endif
            buffer1.push(recv_data1);
        } else if (ret == 0) {
            printf("poll timeout\n");
        }
    }
}

void PCIEDriver::PCIEReadPage(uint8_t *data, uint32_t addr) const {
    if (data == nullptr) {
        perror("PCIEReadPage failed, parameter is null");
        return;
    }
    if(dma_fd < 0){
        perror("PCIEReadPage failed, dma_fd < 0");
        return;
    }
    uint8_t size = pread(dma_fd, data, 4096 * 1080, addr);
    if (size < 0) {
        perror("PCIEReadPage failed");
    }
}

void PCIEDriver::PCIEWriteBram(uint32_t data, uint32_t offset) {
    if (bram_base == nullptr) {
        perror("PCIEWriteBram failed, bram_base is null");
        return;
    }
    volatile uint32_t *reg_ptr;
    reg_ptr = static_cast<volatile uint32_t *>(bram_base) + offset;
    *reg_ptr = (volatile uint32_t &) data;
}

uint32_t PCIEDriver::PCIEReadBram(uint32_t offset) {
    if (bram_base == nullptr) {
        perror("PCIEReadBram failed, bram_base is null");
        return 0;
    }
    volatile uint32_t *reg_ptr;
    reg_ptr = static_cast<uint32_t *>(bram_base) + offset;
    return *reg_ptr;
}