/**
 * @brief 消息服务基类：接收外界发来的消息并处理，需要双方约定好消息格式
 * @author zhangzhiyu
 * @date 2022-09-14
 */

#include "tools/msg_service.h"

MessageService::MessageService(std::string serviceName) : serviceName_(serviceName) {}

void MessageService::init() {
    if (running_) {
        std::cout << "already started!" << std::endl;
        return;
    }

    running_ = true;
    thread_ = std::thread([this]() {
        bool dequeued;
        Content_t message;
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

void MessageService::stop() {
    running_ = false;
    cv.notify_one();
    if (thread_.joinable()) {
        thread_.join();
    }
}

void MessageService::send_msg(Content_t message) {
    queue_.enqueue(message);
    cv.notify_one();
}
