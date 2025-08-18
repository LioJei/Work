/**
* @brief C++11 有界阻塞队列；借鉴了STL的写法
* @author lijunjie
* @date 2025-08-11
*/

#ifndef BLOCKQUEUE_BLOCKQUEUE_HPP
#define BLOCKQUEUE_BLOCKQUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * @brief C++11 有界阻塞队列
 * <br>默认最大容量为 INT32_MAX
 * @tparam T 元素类型
 * @tparam Container 容器类型，默认为std::queue<T>
 */
template<typename T, typename Container = std::queue<T>>
class BlockQueue {
public:
    using size_type = typename Container::size_type;
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using mutex_type = std::mutex;
private:
    Container queue;
    mutable mutex_type mutex;
    std::condition_variable condNotEmpty;
    std::condition_variable condNotFull;
    size_type maxCapacity;

public:

    explicit BlockQueue(size_type capacity = INT32_MAX) : maxCapacity(capacity) {}

    BlockQueue(const BlockQueue &) = delete;

    BlockQueue(BlockQueue &&) = delete;

    BlockQueue &operator=(const BlockQueue &) = delete;

    BlockQueue &operator=(BlockQueue &&) = delete;

    void pop(reference elem) {
        std::unique_lock<mutex_type> lock(mutex);
        // 返回false的时候阻塞
        condNotEmpty.wait(lock, [this]() { return !queue.empty(); });
        elem = std::move(queue.front());
        queue.pop();
        condNotFull.notify_one();
    }

    bool tryPop(reference elem) {
        std::unique_lock<mutex_type> lock(mutex);
        if (queue.empty()) {
            condNotFull.notify_all();
            return false;
        }
        elem = std::move(queue.front());
        queue.pop();
        condNotFull.notify_one();
        return true;
    }

    void push(const value_type &elem) {
        std::unique_lock<mutex_type> lock(mutex);
        condNotFull.wait(lock, [this]() { return queue.size() < maxCapacity; });
        queue.push(elem);
        condNotEmpty.notify_one();
    }

    void push(value_type &&elem) {
        std::unique_lock<mutex_type> lock(mutex);
        condNotFull.wait(lock, [this]() { return queue.size() < maxCapacity; });
        queue.push(std::move(elem));
        condNotEmpty.notify_one();
    }

    bool empty() const {
        std::lock_guard<mutex_type> lock(mutex);
        return queue.empty();
    }

    bool full() const {
        std::lock_guard<mutex_type> lock(mutex);
        return !queue.size() < maxCapacity;
    }

    size_type size() const {
        std::lock_guard<mutex_type> lock(mutex);
        return queue.size();
    }
};

#endif //BLOCKQUEUE_BLOCKQUEUE_HPP
