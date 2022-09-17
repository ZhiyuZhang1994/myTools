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
    virtual void process_msg(Message) override;

private:
    void process_output_msg(Message message);
};

void process_status_bar_msg::process_msg(Message message) {

    TableDriven<uint16_t> tableDriven{
        {1, [this, message]() { process_output_msg(message); }},
    };
    tableDriven.handleKey(1);
}
void process_status_bar_msg::process_output_msg(Message message) {
    std::cout << message << std::endl;
}

int main() {
    process_status_bar_msg service("abc");
    service.init();
    service.send_msg("zzy");

    getchar();
    return 0;
}