/**
 * @brief 此工具实现了观察者机制(同进程内交互):客户不需要针对业务特性实现观察者、通知者类，是一个即拿即用的工具
 * @brief 观察者与通知者做到完全解耦
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-09-08
 */

#include "tools/notifier_and_observer.h"
#include <iostream>
#include <atomic>
#include <utility>
#include <functional>
#include <vector>
#include <string>

namespace Observer_ZZY
{
void SubjectContent::notify(std::string content)
{
    auto observerCallbacks = ObserverContainer::getInstance().queryObserver(subject_);
    for (const auto &each : observerCallbacks) {
        if(!each) {
            std::cout << "nullptr" << std::endl;
            continue;
        }
        each(subject_, content);
    }
}

bool SubjectContent::isObserved() {
    return ObserverContainer::getInstance().isObserved(subject_);
}

CallbackHandler Observer::attach(Subject_t subject, ObserverCallback_t observerCallback)
{
    return ObserverContainer::getInstance().attach(subject, observerCallback);
}

void Observer::detach(Subject_t subject, CallbackHandler handler) {
    ObserverContainer::getInstance().detach(subject, handler);
}

void Notifier::buildSubject(Subject_t subject)
{
    ObserverContainer::getInstance().buildSubject(subject);
}

void Notifier::releaseSubject(Subject_t subject) {
    ObserverContainer::getInstance().releaseSubject(subject);
}

std::shared_ptr<SubjectContent>  Notifier::querySubject(Subject_t subject)
{
    return ObserverContainer::getInstance().querySubject(subject);
}

CallbackHandler ObserverContainer::attach(Subject_t subject, ObserverCallback_t observerCallback)
{
    {
        // 主题未注册则观察失败
        std::unique_lock<std::mutex> lock(subjectMutex_);
        if (subjectContainer_.find(subject) == subjectContainer_.end()) {
            std::cout << "subject: " << subject << " is not exist" << std::endl;
            return -1;  // -1表示注册失败，主题不存在
        }
    }

    {
        // 主题注册后，再添加该主题的观察者
        std::unique_lock<std::mutex> lock(subjectCallbackMutex_);
        auto subjectCbIter = subjectCallbackContainer_.find(subject);
        if (subjectCbIter == subjectCallbackContainer_.end()) {
            subjectCallbackContainer_[subject] = {observerCallback};
            return 0;
        }
        subjectCbIter->second.push_back(observerCallback);
        return subjectCbIter->second.size() - 1; // 返回回调的索引
    }
}

// detach：用于将回调函数至为空指针
void ObserverContainer::detach(Subject_t subject, CallbackHandler handler)
{
    {
        std::unique_lock<std::mutex> lock(subjectMutex_);
        if (subjectContainer_.find(subject) == subjectContainer_.end()) {
            std::cout << "subject: " << subject << " is not exist" << std::endl;
            return;
        }
    }

    {
        std::unique_lock<std::mutex> lock(subjectCallbackMutex_);
        auto subjectCbIter = subjectCallbackContainer_.find(subject);
        if (subjectCbIter == subjectCallbackContainer_.end()) {
            return;
        }
        subjectCbIter->second[handler] = nullptr;
        return;
    }
}

void ObserverContainer::buildSubject(Subject_t subject)
{
    std::unique_lock<std::mutex> lock(subjectMutex_);
    if (subjectContainer_.find(subject) == subjectContainer_.end()) {
        subjectContainer_[subject] = std::make_shared<SubjectContent>(subject);
        std::cout << "buildSubject: " << subject << std::endl;
        return;
    }
    std::cout << "subject: " << subject << "already build" << std::endl;
}

void ObserverContainer::releaseSubject(Subject_t subject) {
    std::unique_lock<std::mutex> lock(subjectMutex_);
    if (subjectContainer_.find(subject) == subjectContainer_.end()) {
        std::cout << "subject: " << subject << " is not exist" << std::endl;
        return;
    }
    subjectContainer_.erase(subject);
    subjectCallbackContainer_.erase(subject);
    std::cout << "subject: " << subject << " is released" << std::endl;
}


std::shared_ptr<SubjectContent> ObserverContainer::querySubject(Subject_t subject)
{
    std::unique_lock<std::mutex> lock(subjectMutex_);
    auto subject_content_iter = subjectContainer_.find(subject);
    if (subject_content_iter == subjectContainer_.end()) {
        return nullptr;
    }
    return subject_content_iter->second;
}

std::vector<ObserverCallback_t> ObserverContainer::queryObserver(Subject_t subject)
{
    std::unique_lock<std::mutex> lock(subjectCallbackMutex_);
    auto subject_cb_iter = subjectCallbackContainer_.find(subject);
    return (subject_cb_iter == subjectCallbackContainer_.end()) ? std::vector<ObserverCallback_t>() : subject_cb_iter->second;
}

bool ObserverContainer::isObserved(Subject_t subject) {
    std::unique_lock<std::mutex> lock(subjectCallbackMutex_);
    auto subject_cb_iter = subjectCallbackContainer_.find(subject);
    return (subject_cb_iter == subjectCallbackContainer_.end()) ? false : true;
}
}