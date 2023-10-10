/**
 * @brief 配置数据中心：实现数据存储、数据订阅、转发功能
 * @brief 支持成员函数作为回调函数
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-07-03
 */

#include "tools/cfgdc/cfgdc.h"
#include <utility>
#include <iostream>
namespace ZZY_TOOLS
{


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


DataCenterBase::DataCenterBase() {
    DataCenterMgr::instance()->registerDataCenter(this);
}

DataCenterBase::~DataCenterBase() {
    // 析构每一个观察者对象
    for (auto& each : callBacks_) {
        for (auto& each : each.second) {
            delete each;
            each = nullptr;
        }
    }
    callBacks_.clear();
}

std::uint32_t DataCenterBase::getDataCenterId() const {
    return dataCenterId_;
}

void DataCenterBase::addObserver(Subject_t subject, CallbackBase* callable, std::uint32_t priority) {
    std::cout << "zzy addObserver " << subject << std::endl;
    auto callBacksIter = callBacks_.find(subject);
    if (callBacksIter == callBacks_.end()) {
        std::cout << "zzy ?" << subject << std::endl;
        callBacks_[subject] = {callable};
    } else {
        std::cout << "zzy ??" << subject << std::endl;
        callBacksIter->second.push_back(callable);
    }
    std::cout << "zzy addObserver111" << subject << std::endl;
}

} // namespace ZZY_TOOLS