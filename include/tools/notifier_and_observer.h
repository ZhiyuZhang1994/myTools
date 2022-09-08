/**
 * @brief 此工具实现了观察者机制(同进程内交互):客户不需要针对业务特性实现观察者、通知者类，是一个即拿即用的工具
 * @brief 观察者与通知者做到完全解耦
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-09-08
 */

#ifndef INCLUDE_TOOLS_NOTIFIER_AND_OBSERVER
#define INCLUDE_TOOLS_NOTIFIER_AND_OBSERVER

#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

namespace Observer_ZZY
{
using Subject_t = std::uint32_t;
using Content_t = std::string;
using CallbackHandler = std::uint32_t;
using InputWrapper = boost::archive::text_iarchive;
using OutputWrapper = boost::archive::text_oarchive;

#define DEFINE_MESSAGE_INPUT_WRAPPER(message) \
    std::stringstream is(message); \
    Observer_ZZY::InputWrapper message##In(is)

#define DEFINE_MESSAGE_OUTPUT_WRAPPER(message) \
    std::stringstream message; \
    Observer_ZZY::OutputWrapper message##Out(message)

// 主题用于发布主题对应信息
class SubjectContent
{
public:
    SubjectContent(Subject_t subject) : subject_(subject) {}

    void notify(Content_t content);

    bool isObserved();

private:
    Subject_t subject_;
};

// 通知者用于建立主题
class Notifier
{
public:
    Notifier() = default;

    Notifier(const Notifier&) = delete;
    Notifier& operator=(const Notifier&) = delete;

    void buildSubject(Subject_t subject);

    void releaseSubject(Subject_t subject);

    std::shared_ptr<SubjectContent> querySubject(Subject_t subject);
};

using ObserverCallback_t = std::function<void(Subject_t, Content_t)>;

// 观察者用于观察主题
class Observer
{
public:
    Observer() = default;

    CallbackHandler attach(Subject_t subject, ObserverCallback_t observerCallback);
    void detach(Subject_t subject, CallbackHandler handler);

    Observer(const Observer&) = delete;
    Observer& operator=(const Observer&) = delete;
};

// 中介者：用于通知者与观察者解耦，对客户不可见，保存两者数据。
class ObserverContainer
{
public:
    using SubjectCallbackContainer_t = std::unordered_map<Subject_t, std::vector<ObserverCallback_t>>;

    using SubjectContainer_t = std::unordered_map<Subject_t, std::shared_ptr<SubjectContent>>;

public:
    static ObserverContainer &getInstance() {
        static ObserverContainer observerContainer;
        return observerContainer;
    }

    CallbackHandler attach(Subject_t subject, ObserverCallback_t observerCallback);
    void detach(Subject_t subject, CallbackHandler handler);

    void buildSubject(Subject_t subject);

    /**
     * @brief 释放主题，同时删掉观察者
     * @param subject
     */
    void releaseSubject(Subject_t subject);

    std::shared_ptr<SubjectContent> querySubject(Subject_t subject);

    std::vector<ObserverCallback_t> queryObserver(Subject_t subject);

    bool isObserved(Subject_t subject);

private:
    ObserverContainer() = default;

private:
    SubjectCallbackContainer_t subjectCallbackContainer_;
    std::mutex subjectCallbackMutex_;

    SubjectContainer_t subjectContainer_;
    std::mutex subjectMutex_;
};

} // namespace Observer_ZZY

#endif