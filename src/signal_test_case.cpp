/**
 * @brief 段错误等信号测试
 * @author zhangzhiyu
 * @date 2022-11-06
 */

#include <utility>
#include <iostream>
#include <signal.h>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>
#include "easylogging/easylogging++.h"

#define ELPP_FEATURE_CRASH_LOG

void signal_callback(int x) {
    std::cout << "signal test" << std::endl;
    signal(SIGSEGV, nullptr);
}

int main()
{
    LOG(INFO) << "My first info log using default logger";
    std::cout << "SSSSSSSSSSSSSSSSSSSSSSSSSSS" << std::endl;

    // signal(SIGSEGV, signal_callback);

    int *b = nullptr;
    *b = 2;
    getchar();
    return 0;
}