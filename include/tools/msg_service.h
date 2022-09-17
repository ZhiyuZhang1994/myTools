/**
 * @brief 消息服务基类：接收外界发来的消息并处理，需要双方约定好消息格式
 * @author zhangzhiyu
 * @date 2022-09-14
 */

#include "safe_queue.h"
#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <future>
#include <string>
#include <condition_variable>
#include <iostream>

using Content_t = std::string;
using InputWrapper = boost::archive::text_iarchive;
using OutputWrapper = boost::archive::text_oarchive;

#define DEFINE_MESSAGE_INPUT_WRAPPER(message) \
    std::stringstream is(message); \
    InputWrapper message##In(is)
#define DEFINE_MESSAGE_OUTPUT_WRAPPER(message) \
    std::stringstream message; \
    OutputWrapper message##Out(message)
class MessageService {
public:
    explicit MessageService(std::string serviceName);

    ~MessageService() {
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

    MessageService(const MessageService&) = delete;
    MessageService& operator=(const MessageService&) = delete;
};
