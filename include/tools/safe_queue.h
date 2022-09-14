#ifndef BEFEMLIB_SAFE_QUEUE_H
#define BEFEMLIB_SAFE_QUEUE_H

#include <mutex>
#include <queue>

/**
 * @brief 线程安全的消息队列
 * @tparam T 队列数据类型
 * @author Nemo
 */
template<typename T>
class SafeQueue {
public:
    SafeQueue() = default;
    SafeQueue(const SafeQueue &other) = delete;
    SafeQueue(SafeQueue &&other) = delete;
    ~SafeQueue() = default;

    /**
     * @brief 判断队列是否为空
     * @return
     */
    bool empty() {
        std::unique_lock<std::mutex> lock(mutex_);

        return queue_.empty();
    }

    /**
     * @brief 队列当前大小
     * @return
     */
    int size() {
        std::unique_lock<std::mutex> lock(mutex_);

        return queue_.size();
    }

    /**
     * @brief 入队
     * @param t
     */
    void enqueue(T &t) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.emplace(t);
    }

    /**
     * @brief 出队
     * @param t
     * @return
     */
    bool dequeue(T &t) {
        std::unique_lock<std::mutex> lock(mutex_);

        if (queue_.empty())
            return false;
        t = std::move(queue_.front());

        queue_.pop();

        return true;
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
};

#endif // BEFEMLIB_SAFE_QUEUE_H