/**
 * @brief 消息线程服务：独立线程，接收消息，处理消息。需要与发送方约定好数据格式
 * @author zhangzhiyu
 * @date 2022-09-16
 */

#include "tools/tassk_class.h"
#include <utility>
#include <iostream>

int main() {
    EndlessRunService service("abc");
    service.submit([](){std::cout << "test endless run func" << std::endl;});
    std::this_thread::sleep_for(std::chrono::seconds(3));
    service.endless_run();

    getchar();
    return 0;
}