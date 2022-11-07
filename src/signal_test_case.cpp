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
#include <iostream>
#include <execinfo.h>

void printCallStack()
{
    int size = 16;                                 // 最多打印16层调用栈
    void *buffer[size] = {nullptr};                // 用于存放调用栈信息
    int count = backtrace(buffer, size);           // 获取调用栈
    char **ptr = backtrace_symbols(buffer, count); // 将调用栈信息转成字符串
    for (int i = 0; i < count; i++)
    { // 打印字符串
        std::cout << ptr[i] << std::endl;
    }
    free(ptr); // 释放分配的内存
}

void signal_callback(int x)
{
    std::cout << "signal: " << x << std::endl;
    printCallStack();
}

void test3(int n)
{
    int *b = nullptr;
    *b = 2;
}
void test2(int n) { test3(3); }
void test1(int n) { test2(2); }

int main()
{
    std::cout << "SSSSSSSSSSSSSSSSSSSSSSSSSSS" << std::endl;
    signal(SIGSEGV, signal_callback);
    test1(1);

    getchar();
    return 0;
}