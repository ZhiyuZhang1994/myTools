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


/**
 * @brief 注册观察者回调函数的对外接口
 * 
 * @tparam U 回调函数所在的类类型的指针
 * @tparam T 回调函数所在的类类型
 * @param subject 订阅的主题
 * @param observer 回调函数所在的类实例的指针
 * @param callback 回调函数的指针
 * @param priority 回调函数的优先级
 */
// template<class U, class T>
// void AddObserver(Subject_t subject, U observer,
//                           void (T::*callback)(Subject_t subject, Content_t content), std::uint32_t priority = 50) {
//     MemberFunctionCallback<T>* callable = new MemberFunctionCallback<T>(observer, callback);
//     DataCenterMgr::instance()->getDataCenter(subject);

//     // DataCenterMgr::instance()->addObserver(subject, callable, priority);
// }




#define ADD_MEMBER_FUNCTION_CALLBACK(subject, callback, ...)                            \
    DataCenterBase* center = DataCenterMgr::instance()->getDataCenter(subject); \
    center->AddObserver(subject, this, callback,  ##__VA_ARGS__);

Subject_t ZZY_TEST = 0;

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


class DataCenter : public DataCenterBase {
public:
    DataCenter() = default;
};

int main() {
    DataCenter aaaa;

    A a;
    a.init();
}

