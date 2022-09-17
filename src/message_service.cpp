/**
 * @brief 消息线程服务：独立线程，接收消息，处理消息。需要与发送方约定好数据格式
 * @author zhangzhiyu
 * @date 2022-09-16
 */

#include "tools/tassk_class.h"
#include "tools/msg_service.h"
#include <utility>
#include <iostream>

class process_status_bar_msg : public MessageService {
public:
    process_status_bar_msg(std::string className) : MessageService(className) {}
    virtual void process_msg(Message) override;

};

void process_status_bar_msg::process_msg(Message message) {
    
}


int main() {
    EndlessRunService service("abc");
    service.submit([](){std::cout << "test endless run func" << std::endl;});
    std::this_thread::sleep_for(std::chrono::seconds(3));
    service.endless_run();

    getchar();
    return 0;
}