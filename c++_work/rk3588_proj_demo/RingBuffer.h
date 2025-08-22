/**
 * @author <lijunjie>
 * @date 2025-08-19
 * @brief This file defines the interface of RingBuffer.
 *
 * */
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cstring>

constexpr size_t WIDTH = 1920;  // 图像宽度
constexpr size_t HEIGHT = 1080;  // 图像高度
constexpr size_t BYTES_PER_PIXEL = 2;    // 像素字节数//数据定义
constexpr size_t PCIE_BUFFER_LINE_SIZE = WIDTH * BYTES_PER_PIXEL;  // 1080P分辨率下，一帧YUV数据一行占用空间大小
constexpr size_t PCIE_BUFFER_FRAME_SIZE = WIDTH * HEIGHT * BYTES_PER_PIXEL;  // 1080P分辨率下，一帧YUV数据一行占用空间大小
constexpr size_t PCIE_BUFFER_SIZE = 4096 * HEIGHT;  // pcie缓存区大小
constexpr size_t BUFFER_COUNT = 10;  // 缓存块数量;

using FrameType = std::array<unsigned char, PCIE_BUFFER_FRAME_SIZE>;
template <typename T>
class RingBuffer {
private:
    std::vector<T> buffer;          // 存储数据的缓冲区
    size_t head;                    // 缓冲区头部索引
    size_t tail;                    // 缓冲区尾部索引
    size_t capacity;                // 缓冲区容量
    bool full;                      // 缓冲区是否已满
    mutable std::mutex mtx;         // 互斥锁，用于线程安全
    std::condition_variable not_full;  // 缓冲区未满的条件变量
    std::condition_variable not_empty; // 缓冲区非空的条件变量

public:
    // 构造函数
    explicit RingBuffer(size_t size) : head(0), tail(0), capacity(size), full(false) {
        buffer.resize(size);
    }

    // 析构函数
    ~RingBuffer() = default;

    // 向缓冲区写入数据
    void put(const T& item) {
        std::unique_lock<std::mutex> lock(mtx);

        // 等待直到缓冲区未满
        not_full.wait(lock, [this]() { return !isFull(); });

        buffer[tail] = item;
        tail = (tail + 1) % capacity;

        if (tail == head) {
            full = true;
        }

        lock.unlock();
        not_empty.notify_one();  // 通知消费者缓冲区非空
    }

    // 从缓冲区读取数据
    T get() {
        std::unique_lock<std::mutex> lock(mtx);

        // 等待直到缓冲区非空
        not_empty.wait(lock, [this]() { return !isEmpty(); });

        T item = buffer[head];
        head = (head + 1) % capacity;
        full = false;

        lock.unlock();
        not_full.notify_one();  // 通知生产者缓冲区未满
        return item;
    }

    // 检查缓冲区是否为空
    bool isEmpty() const {
        return (!full && (head == tail));
    }

    // 检查缓冲区是否已满
    bool isFull() const {
        return full;
    }

    // 获取缓冲区的大小
    size_t size() const {
        if (full) {
            return capacity;
        }
        if (tail >= head) {
            return tail - head;
        }
        return capacity + tail - head;
    }
};

class FrameRingBuffer {
private:
    static constexpr size_t FRAME_WIDTH = 1920;
    static constexpr size_t FRAME_HEIGHT = 1080;
    static constexpr size_t BYTES_PER_PIXEL = 2;
    static constexpr size_t FRAME_SIZE = FRAME_WIDTH * FRAME_HEIGHT * BYTES_PER_PIXEL; // 4,147,200 bytes
    static constexpr size_t PCIE_BUFFER_SIZE = 4096 * HEIGHT;  // pcie缓存区大小
    static constexpr size_t BUFFER_SIZE = 10; // 存放10帧数据

    // 使用连续内存存储所有帧数据
    uint8_t *buffer;
    std::vector<bool> frame_valid; // 标记每帧数据是否有效

    size_t read_index;
    size_t write_index;
    std::atomic<size_t> count;

    mutable std::mutex mutex;
    std::condition_variable not_full;
    std::condition_variable not_empty;

public:
    FrameRingBuffer()
            : buffer(nullptr),
              frame_valid(BUFFER_SIZE, false),
              read_index(0),
              write_index(0),
              count(0) {
        posix_memalign((void**)&buffer, 4096, PCIE_BUFFER_SIZE * BUFFER_SIZE );
    }

    // 向缓冲区写入一帧数据（阻塞）
    bool push(const uint8_t* frame_data) {
        std::unique_lock<std::mutex> lock(mutex);

        // 等待缓冲区有空间
        not_full.wait(lock, [this] { return count < BUFFER_SIZE; });

        // 计算写入位置
        size_t write_pos = write_index * PCIE_BUFFER_SIZE;

        // 复制数据到缓冲区
        memcpy(buffer + write_pos, frame_data, PCIE_BUFFER_SIZE);
        frame_valid[write_index] = true;

        // 更新写指针和计数
        write_index = (write_index + 1) % BUFFER_SIZE;
        count++;

        // 通知读者有新数据
        lock.unlock();
        not_empty.notify_one();
        return true;
    }

    // 从缓冲区读取一帧数据（阻塞）
    bool pop(uint8_t* frame_data) {
        std::unique_lock<std::mutex> lock(mutex);

        // 等待缓冲区有数据
        not_empty.wait(lock, [this] { return count > 0; });

        // 计算读取位置
        size_t read_pos = read_index * PCIE_BUFFER_SIZE;

        // 复制数据到输出缓冲区
        memcpy(frame_data, buffer + read_pos, PCIE_BUFFER_SIZE);
        frame_valid[read_index] = false;

        // 更新读指针和计数
        read_index = (read_index + 1) % BUFFER_SIZE;
        count--;

        // 通知写者有空间
        lock.unlock();
        not_full.notify_one();
        return true;
    }

    // 非阻塞写入
    bool try_push(const uint8_t* frame_data) {
        std::unique_lock<std::mutex> lock(mutex);
        if (count >= BUFFER_SIZE) {
            return false; // 缓冲区满
        }

        // 计算写入位置
        size_t write_pos = write_index * PCIE_BUFFER_SIZE;

        // 复制数据到缓冲区
        memcpy(buffer + write_pos, frame_data, PCIE_BUFFER_SIZE);
        frame_valid[write_index] = true;

        // 更新写指针和计数
        write_index = (write_index + 1) % BUFFER_SIZE;
        count++;

        lock.unlock();
        not_empty.notify_one();
        return true;
    }

    // 非阻塞读取
    bool try_pop(uint8_t* frame_data) {
        std::unique_lock<std::mutex> lock(mutex);
        if (count <= 0) {
            return false; // 缓冲区空
        }

        // 计算读取位置
        size_t read_pos = read_index * PCIE_BUFFER_SIZE;

        // 复制数据到输出缓冲区
        memcpy(frame_data, buffer + read_pos, PCIE_BUFFER_SIZE);
        frame_valid[read_index] = false;

        // 更新读指针和计数
        read_index = (read_index + 1) % BUFFER_SIZE;
        count--;

        lock.unlock();
        not_full.notify_one();
        return true;
    }

    // 获取缓冲区总容量（帧数）
    size_t capacity() const {
        return BUFFER_SIZE;
    }

    // 获取当前存储的有效帧数
    size_t size() const {
        return count.load();
    }

    // 检查缓冲区是否为空
    bool empty() const {
        return count.load() == 0;
    }

    // 检查缓冲区是否已满
    bool full() const {
        return count.load() >= BUFFER_SIZE;
    }

    // 清空缓冲区
    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        read_index = 0;
        write_index = 0;
        count = 0;
        std::fill(frame_valid.begin(), frame_valid.end(), false);
    }

    // 预分配内存的帧数据缓冲区
    static constexpr size_t getFrameSize() {
        return PCIE_BUFFER_SIZE;
    }
    ~FrameRingBuffer() {
        if (buffer) {
            free(buffer);
            buffer = nullptr;
        }
    }
};