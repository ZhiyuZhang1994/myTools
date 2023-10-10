/**
 * @brief 配置数据中心：实现数据存储、数据订阅、转发功能
 * @brief 支持成员函数作为回调函数
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-07-03
 */

#include <iostream>
#include <unordered_map>
#include <vector>

#ifndef TOOLS_CFGDC_CFGDC_H
#define TOOLS_CFGDC_CFGDC_H

namespace ZZY_TOOLS
{

/**
 * @brief 类成员函数作为回调函数的管理类
 */
using Subject_t = std::uint32_t;
using Content_t = const void*; // 标识为const提示客户不可修改消息内容


/**
 * @brief 回调函数基类：拥有统一的回调函数执行接口
 * 用作不同类成员函数做回调时，作为基类指针可以访问任意子类
 */
class CallbackBase {
public:
    virtual ~CallbackBase() = default;

    virtual bool operator()(Subject_t subject, Content_t message) = 0;
};

/**
 * @brief 回调函数子类：实现统一的回调函数执行接口
 * 保存各回调函数的信息
 * 
 * @tparam T 回调函数的类名
 * @param callback 回调函数的函数名
 */
template<class T>
class MemberFunctionCallback : public CallbackBase {
public:
    MemberFunctionCallback(T* pointer, void (T::*callback)(Subject_t subject,  Content_t content)) {
        pointer_ = pointer;
        memberFunc_ = callback;
    }
    ~MemberFunctionCallback() override = default;

    bool operator()(Subject_t subject, Content_t message) override {
        if (!memberFunc_) {
            return false;
        }
        // 指针到成员运算符 `->*` 是C++中用于调用成员函数指针所指向的成员函数的运算符。
        (pointer_->*memberFunc_)(subject, message);
        return true;
    }

private:
    T* pointer_ = nullptr;
    void (T::*memberFunc_)(Subject_t subject, Content_t content);
};



/**
 * @brief 数据中心基类，提供以下功能：
 * 1、添加该数据中心对应主题的观察者
 * 2、不提供删除观察者接口，认为观察一个数据变化是持续需要的
 */
class DataCenterMgr;

class DataCenterBase {
public:
    DataCenterBase();

    virtual ~DataCenterBase();

    /**
     * @brief 获取该数据中心的唯一编号
     * 
     * @return std::uint32_t 唯一编号
     */
    std::uint32_t getDataCenterId() const;

    /**
     * @brief 添加观察者
     * 
     * @param subject 主题
     * @param callable 观察者对象
     * @param priority 该观察者的优先级
     */
    void addObserver(Subject_t subject, CallbackBase* callable, std::uint32_t priority);

    template<class U, class T>
    void AddObserver(Subject_t subject, U observer,
                          void (T::*callback)(Subject_t subject, Content_t content), std::uint32_t priority = 50) {
        MemberFunctionCallback<T>* callable = new MemberFunctionCallback<T>(observer, callback);
        addObserver(subject, callable, priority);
    }

    /**
     * @brief 对不通过
     * 
     */
    void notifyObserver() {
        for (auto each : callBacks_[0]) {
            std::uint32_t aa = 123;
            each->operator()(0, &aa);
        }
    }

protected:
    std::uint32_t dataCenterId_ = 0;
    std::unordered_map<Subject_t, std::vector<CallbackBase*>> callBacks_;
};

/**
 * @brief 管理所有的数据中心
 */
class DataCenterMgr {
public:
    static DataCenterMgr* instance() {
        static DataCenterMgr mgr;
        return &mgr;
    }

    void registerDataCenter(DataCenterBase* dataCenter) {
        std::cout << "zzy registerDataCenter" << std::endl;
        dataCenters[dataCenter->getDataCenterId()] = dataCenter;
    }

    DataCenterBase* getDataCenter(std::uint32_t dataCenterId) {
        return dataCenters[dataCenterId];
    }

private:
    DataCenterMgr() = default;

protected:
    std::unordered_map<std::uint32_t, DataCenterBase*> dataCenters;
};

} // namespace ZZY_TOOLS

#endif