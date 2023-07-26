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

Subject_t FIRST_SUBJECT = 100;
Subject_t SECOND_SUBJECT = 101;

class DataCenterMgr : public DataCenterMgrBase {
public:
    static DataCenterMgr* instance() {
        static DataCenterMgr mgr;
        return &mgr;
    }

    // ######################################业务接口###############################
    // 调用此成员函数，必然知道此函数对应的观察主题
    void addData(std::uint32_t key, std::uint32_t value) {
        auto dataIter = toBeMonitoredData_.find(key);
        if (dataIter == toBeMonitoredData_.end()) {
            toBeMonitoredData_[key] = {value};
        } else {
            dataIter->second.push_back(value);
        }
        auto& callbacks = callBacks_[FIRST_SUBJECT];
        for (auto& each : callbacks) {
            (*each)(1, nullptr);
        }
    }

private:
    DataCenterMgr(){}
    virtual ~DataCenterMgr(){}


private:
    std::unordered_map<std::uint32_t, std::vector<std::uint32_t>> toBeMonitoredData_;
};


template<class U, class T>
void AddObserver(Subject_t subject, U observer,
                          void (T::*callback)(Subject_t subject, Content_t content), std::uint32_t priority = 50) {
    MemberFunctionCallback<T>* callable = new MemberFunctionCallback<T>(observer, callback);
    DataCenterMgr::instance()->addObserver(subject, callable, priority);
}

#define ADD_MEMBER_FUNCTION_CALLBACK(subject, callback, ...)                            \
    AddObserver(subject, this, callback, ##__VA_ARGS__);


/**
 * @brief 客户类A：关注时间
 */
class ClassA {
public:
    ClassA() {}
    void observer(Subject_t subject, Content_t message) {
        std::cout << "receive subject: " << subject << std::endl;
    }
    void init() {
        ADD_MEMBER_FUNCTION_CALLBACK(FIRST_SUBJECT, &ClassA::observer);
        // ADD_MEMBER_FUNCTION_CALLBACK(FIRST_SUBJECT, &A::observer, 20);
    }

};

class A {
public:
    A() {}
    void observer(Subject_t subject, Content_t message) {
        std::cout << "receive subject: " << subject << std::endl;
    }
    void init() {
        ADD_MEMBER_FUNCTION_CALLBACK(FIRST_SUBJECT, &A::observer);
        // ADD_MEMBER_FUNCTION_CALLBACK(FIRST_SUBJECT, &A::observer, 20);
    }

};

int main() {
    A a;
    a.init();
    DataCenterMgr::instance()->addData(1,2);
}

