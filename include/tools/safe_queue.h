/**
 * @brief 线程安全的消息队列
 * @tparam T 队列数据类型
 * @author https://wangpengcheng.github.io/2019/05/17/cplusplus_theadpool/
 */

#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <shared_mutex>
#include <queue>
#include <memory>
#include <mutex>

namespace ZZY_TOOLS
{
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
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return queue_.empty();
    }

    /**
     * @brief 队列当前大小
     * @return
     */
    int size() {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return queue_.size();
    }

    /**
     * @brief 入队(引用)
     * @param t
     */
    void enqueue(T &t) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        queue_.emplace(t);
    }

    /**
     * @brief 入队(右值)
     * @param t
     */
    void enqueue(T &&t) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        queue_.emplace(std::move(t));
    }

    /**
     * @brief 出队
     * @param t
     * @return
     */
    bool dequeue(T &t) {
        std::unique_lock<std::shared_mutex> lock(mutex_);

        if (queue_.empty()) {
            return false;
        }
        t = std::move(queue_.front());

        queue_.pop();

        return true;
    }

private:
    std::queue<T> queue_;
    std::shared_mutex mutex_;
};

}
#endif // SAFE_QUEUE_H