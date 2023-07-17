/**
 * @brief 配置数据中心：实现数据存储、数据订阅、转发功能
 * @brief 支持成员函数作为回调函数
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-07-03
 */

#include <iostream>
#include <unordered_map>
#include <vector>

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


// 数据中心存储观察者的入口
class DataCenterMgrBase {
public:
    static DataCenterMgrBase* instance() {
        static DataCenterMgrBase mgr;
        return &mgr;
    }

    DataCenterMgrBase(){}

    void addObserver(Subject_t subject, CallbackBase* callable, std::uint32_t priority) {
        // 根据subject查找对应的子mgr
    }

private:
    std::unordered_map<Subject_t, std::vector<CallbackBase*>> callBacks_;
};

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
template<class U, class T>
void AddObserver(Subject_t subject, U observer,
                          void (T::*callback)(Subject_t subject, Content_t content), std::uint32_t priority = 50) {
    MemberFunctionCallback<T>* callable = new MemberFunctionCallback<T>(observer, callback);
    DataCenterMgrBase::instance()->addObserver(subject, callable, priority);
}

#define ADD_MEMBER_FUNCTION_CALLBACK(subject, callback, ...)                            \
    AddObserver(subject, this, callback, ##__VA_ARGS__);

} // namespace ZZY_TOOLS