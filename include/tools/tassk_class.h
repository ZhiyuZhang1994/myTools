/**
 * @brief endless运行的任务，拥有消息队列，处理外部发送来的消息
 * @author zhangzhiyu
 * @date 2022-09-14
 */

#ifndef INCLUDE_TOOLS_TASK_CLASS_H
#define INCLUDE_TOOLS_TASK_CLASS_H


#include "safe_queue.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <future>
#include <string>
#include <condition_variable>
#include <iostream>

namespace ZZY_TOOLS
{
class EndlessRunService {
public:
    EndlessRunService(std::string serviceName);

    ~EndlessRunService() {
        stop();
    }

    void stop();

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

        cv.notify_one();
        return taskPtr->get_future();
    }
    
private:
    std::string serviceName_;
    std::thread thread_;
    std::atomic<bool> running_{false};
    SafeQueue<std::function<void()>> queue_; // 任务队列
    std::mutex mtx;
    std::condition_variable cv;
};


EndlessRunService::EndlessRunService(std::string serviceName) : serviceName_(serviceName) {}

void EndlessRunService::endless_run() {
    if (running_) {
        std::cout << "already started!" << std::endl;
        return;
    }

    running_ = true;
    thread_ = std::thread([this]() {
        std::function<void()> func;
        bool dequeued;
        while (running_) {
            {
                // 加锁等待任务入队
                std::unique_lock<std::mutex> lock(mtx);
                if (queue_.empty()) {
                    cv.wait(lock);
                }
                dequeued = queue_.dequeue(func);
            }
            if (dequeued) {
                func();
            }
        }
    });
}

void EndlessRunService::stop() {
    running_ = false;
    cv.notify_one();
    if (thread_.joinable()) {
        thread_.join();
    }
}

}

#endif