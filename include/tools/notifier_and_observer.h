// description: 观察者机制
// author: zhangzhiyu
// date: 2022.4.23

// 介绍：此工具实现了观察者机制(同进程内交互)：
// 客户不需要针对业务特性实现观察者、通知者类，是一个即拿即用的工具
// 观察者与通知者相互解耦。
// 当前缺陷：
//      1、未加锁，不保证线程安全
//      2、消息内容固定为int，无法扩展：制约了回调函数与通知函数
//      3、未考虑性能问题，只是玩具级工具
//      4、未实现解观察detach()与释放主题release()两函数

#ifndef INCLUDE_TOOLS_NOTIFIER_AND_OBSERVER
#define INCLUDE_TOOLS_NOTIFIER_AND_OBSERVER

#include <iostream>
#include <atomic>
#include <utility>
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "class_register.h"

using subject_t = uint16_t;

// 主题用于发布主题对应信息
class subject_content
{
public:
    subject_content(subject_t subject) : subject_(subject) {}

    void notify(std::vector<int> content);

private:
    subject_t subject_;
};

// 通知者用于建立主题
class notifier
{
public:
    notifier() = default;

    DISALLOW_COPY_AND_ASSIGN(notifier);

    void build_subject(subject_t subject);

    std::shared_ptr<subject_content> query_subject(subject_t subject);
};

// 暂定回调函数入参为主题类型与值为int组成的vector，之后可扩展
using observer_cb_t = std::function<void(subject_t, std::vector<int>)>;

// 观察者用于观察主题
class observer
{
public:
    observer() = default;

    void attach(subject_t subject, observer_cb_t observer_cb);

    DISALLOW_COPY_AND_ASSIGN(observer);
};

// 中介者：用于通知者与观察者解耦，对客户不可见，保存两者数据。
class observer_container
{
public:
    using subject_cb_container_t = std::unordered_map<subject_t, std::vector<observer_cb_t>>;

    using subject_container_t = std::unordered_map<subject_t, std::shared_ptr<subject_content>>;

public:
    SINGLETON_CLASS(observer_container);

    void attach(subject_t subject, observer_cb_t observer_cb);

    void build_subject(subject_t subject);

    std::shared_ptr<subject_content> query_subject(subject_t subject);

    std::vector<observer_cb_t> query_observer(subject_t subject);

private:
    observer_container() = default;

private:
    subject_cb_container_t subject_cb_container;

    subject_container_t subject_container;
};

#endif