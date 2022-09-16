/**
 * @brief 消息服务基类：接收外界发来的消息并处理，需要双方约定好消息格式
 * @author zhangzhiyu
 * @date 2022-09-14
 */

#include "safe_queue.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <future>
#include <string>
#include <condition_variable>
#include <iostream>

class EndlessRunService {
public:
    using Message = std::string;

public:
    EndlessRunService(std::string serviceName);

    ~EndlessRunService() {
        stop();
    }

    std::string serviceName() {return serviceName_};

    // 启动消息线程
    void init();

    // 停止消息线程
    void stop();

    // 外接发送消息：需持有对应实例指针，指针要存放在服务管理仓库中。
    void send_msg(Message message);

    // 处理消息：子类继承该函数，不同子类处理不同消息
    virtual void process_msg(Message) {}

private:
    std::string serviceName_;
    std::thread thread_;
    std::atomic<bool> running_{false};
    SafeQueue<Message> queue_;  // 消息邮箱
    std::mutex mtx;
    std::condition_variable cv;
};

EndlessRunService::EndlessRunService(std::string serviceName) : serviceName_(serviceName) {}

void EndlessRunService::init() {
    if (running_) {
        std::cout << "already started!" << std::endl;
        return;
    }

    running_ = true;
    thread_ = std::thread([this]() {
        bool dequeued;
        Message message;
        while (running_) {
            {
                // 加锁等待任务入队
                std::unique_lock<std::mutex> lock(mtx);
                if (queue_.empty()) {
                    cv.wait(lock);
                }
                dequeued = queue_.dequeue(message);
            }
            if (dequeued) {
                process_msg(message);
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

void EndlessRunService::send_msg(Message message) {
    queue_.enqueue(message);
    cv.notify_one();
}
