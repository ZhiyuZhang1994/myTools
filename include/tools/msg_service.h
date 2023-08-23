/**
 * @brief 消息服务基类：接收外界发来的消息并处理，需要双方约定好消息格式
 * @author zhangzhiyu
 * @date 2022-09-14
 */

#ifndef TOOLS_MSG_SERVICES_H
#define TOOLS_MSG_SERVICES_H

#include "safe_queue.h"
// #include "boost_serialize_define.h"
#include "tools/hash_func.h"
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
using Content_t = std::string;

class MsgService {
public:
    explicit MsgService(std::string serviceName);

    ~MsgService() {
        stop();
    }

    std::string serviceName() { return serviceName_; };

    // 启动消息线程
    void init();

    // 停止消息线程
    void stop();

    // 外接发送消息：需持有对应实例指针，指针要存放在服务管理仓库中。
    void send_msg(Content_t message);

    // 处理消息：子类继承该函数，不同子类处理不同消息
    virtual void process_msg(Content_t) {}

protected:
    std::string serviceName_;
    std::thread thread_;
    std::atomic<bool> running_{false};
    SafeQueue<Content_t> queue_;  // 消息邮箱
    std::mutex mtx;
    std::condition_variable cv;

    MsgService(const MsgService&) = delete;
    MsgService& operator=(const MsgService&) = delete;
};

using ServiceID_t = std::uint32_t;

// 消息服务仓库：存储所有消息服务
class MsgServiceRepo {
public:

public:
    static MsgServiceRepo& instance() {
        static MsgServiceRepo repo;
        return repo;
    }

    // 注册类
    void sign(ServiceID_t serviceId, std::shared_ptr<MsgService> servicePtr) {
        std::unique_lock<std::mutex> lock(servicesPtrMutex_);
        if (servicesPtrContainer.find(serviceId) == servicesPtrContainer.end()) {
            servicesPtrContainer[serviceId] = servicePtr;
        }
    }

    std::shared_ptr<MsgService> getServicePtr(ServiceID_t serviceId) {
        std::unique_lock<std::mutex> lock(servicesPtrMutex_);
        auto iter = servicesPtrContainer.find(serviceId);
        return (iter == servicesPtrContainer.end()) ? nullptr : iter->second;
    }

private:
    MsgServiceRepo() = default;

private:
    std::unordered_map<ServiceID_t, std::shared_ptr<MsgService>> servicesPtrContainer;
    std::mutex servicesPtrMutex_;
};

// 类自注册宏
#define MESSAGE_SERVICE_REGISTER(class_name, serviceId, ...)                                   \
    std::shared_ptr<class_name> servicePtr = std::make_shared<class_name>(__VA_ARGS__);        \
    MsgServiceRepo::instance().sign(serviceId, servicePtr)

#define GET_MESSAGE_SERVICE_PTR(class_name, serviceId) \
    std::dynamic_pointer_cast<class_name>(MsgServiceRepo::instance().getServicePtr(serviceId))

}

#endif