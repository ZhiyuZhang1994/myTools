/**
 * @brief 配置数据中心：实现数据存储、数据订阅、转发功能
 * @brief 支持成员函数作为回调函数
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-07-03
 */

#include "tools/cfgdc/cfgdc.h"
#include <utility>
#include <iostream>

using namespace ZZY_TOOLS;

#define ADD_MEMBER_FUNCTION_CALLBACK(subject, callback, ...)                            \
    AddObserver(subject, this, callback, ##__VA_ARGS__);

Subject_t ZZY_TEST = 100;

class A {
public:
    A() {}
    void observer(Subject_t subject, Content_t message) {
        std::cout << "receive subject: " << subject << "msg: " << message << std::endl;
    }
    void init() {
        ADD_MEMBER_FUNCTION_CALLBACK(ZZY_TEST, &A::observer);
        // ADD_MEMBER_FUNCTION_CALLBACK(ZZY_TEST, &A::observer, 20);
    }

};

int main() {
    A a;
    a.init();
}

