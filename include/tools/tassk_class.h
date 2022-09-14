/**
 * @brief endless运行的任务，拥有消息队列，处理外部发送来的消息
 * @author zhangzhiyu
 * @date 2022-09-14
 */

#include "safe_queue.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <future>


class EndlessRunService {
public:
    EndlessRunService() = default;

    ~EndlessRunService() {
        shutdown_ = true;
        cvPool_.notify_all();
        t_.join();
    }

    void endless_run();
    /**
     * @brief 异步提交任务到邮箱
     * @tparam F 任务类型
     * @tparam Args 任务类型额外参数类型
     * @param f 任务
     * @param args 任务额外参数
     * @return 任务的future以便获得任务执行结果
     */
    template<typename F, typename... Args>
    auto submit(F &&f, Args &&...args) -> std::future<decltype(f(args...))> {
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        std::function<void()> wrapperFunc = [taskPtr]() {
            (*taskPtr)();
        };
        queue_.enqueue(wrapperFunc);

        cvPool_.notify_one();
        return taskPtr->get_future();
    }
    
private:
    std::thread t_;
    std::atomic<bool> running_{false};
    SafeQueue<std::function<void()>> queue_;
    // 任务队列互斥量
    std::mutex mutQueue_;
    // 线程环境锁
    std::condition_variable cvPool_;
    // 线程是否关闭
    bool shutdown_;
};

void EndlessRunService::endless_run() {
    t_ = std::thread([this]() {
        std::function<void()> func;
        bool dequeued;
        while (!shutdown_) {
            {
                // 加锁等待任务入队
                std::unique_lock<std::mutex> lock(mutQueue_);
                if (queue_.empty()) {
                    cvPool_.wait(lock);
                }
                dequeued = queue_.dequeue(func);
            }
            if (dequeued) {
                func();
            }
        }
    });
}

