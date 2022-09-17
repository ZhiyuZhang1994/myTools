/**
 * @brief 消息线程服务：独立线程，接收消息，处理消息。需要与发送方约定好数据格式
 * @author zhangzhiyu
 * @date 2022-09-16
 */

#include "tools/tassk_class.h"
#include "tools/msg_service.h"
#include "tools/table_driven.h"

#include <utility>
#include <iostream>

class process_status_bar_msg : public MessageService {
public:
    explicit process_status_bar_msg(std::string serviceName) : MessageService(serviceName) {}
    virtual void process_msg(Content_t) override;

private:
    void process_output_msg(InputWrapper& messageIn);
};

void process_status_bar_msg::process_msg(Content_t message) {
    DEFINE_MESSAGE_INPUT_WRAPPER(message);
    std::uint16_t messageType = 0;
    messageIn >> messageType;

    std::cout << "process msg" << messageType << std::endl;

    TableDriven<uint16_t> tableDriven{
        {1, [this, &messageIn]() { process_output_msg(messageIn); }},
    };
    tableDriven.handleKey(messageType);
}
void process_status_bar_msg::process_output_msg(InputWrapper& messageIn) {
    std::cout << "process msg" << 1 << std::endl;
    std::uint16_t id = 0;
    messageIn >> id;
    std::cout << id << std::endl;
}

int main() {
    process_status_bar_msg service("abc");
    service.init();
    int d1 = 1;
    int d2 = 1000;
    DEFINE_MESSAGE_OUTPUT_WRAPPER(message);
    messageOut << d1 << d2;//序列化到一个ostringstream里面
    service.send_msg(message.str());

    getchar();
    return 0;
}