// description: 观察者机制
// author: zhangzhiyu
// date: 2022.4.23

#include "../../include/tools/notifier_and_observer.h"
#include <iostream>
#include <atomic>
#include <utility>
#include <functional>
#include <vector>

void subject_content::notify(std::vector<int> content)
{
    auto observer_cbs = observer_container::get_instance()->query_observer(subject_);
    for (auto &each : observer_cbs) {
        each(subject_, content);
    }
}

void observer::attach(subject_t subject, observer_cb_t observer_cb)
{
    observer_container::get_instance()->attach(subject, observer_cb);
}

void notifier::build_subject(subject_t subject)
{
    observer_container::get_instance()->build_subject(subject);
}

std::shared_ptr<subject_content>  notifier::query_subject(subject_t subject)
{
    return observer_container::get_instance()->query_subject(subject);
}

void observer_container::attach(subject_t subject, observer_cb_t observer_cb)
{
    // 主题未注册则观察失败
    if (subject_container.find(subject) == subject_container.end()) {
        std::cout << "subject: " << subject << " is not build" << std::endl;
        return;
    }

    // 主题注册后，再添加该主题的观察者
    auto subjectcb_iter = subject_cb_container.find(subject);
    if (subjectcb_iter == subject_cb_container.end()) {
        subject_cb_container[subject] = {observer_cb};
    } else {
        subjectcb_iter->second.push_back(observer_cb);
    }
    std::cout << "attach subject: " << subject << std::endl;

}

void observer_container::build_subject(subject_t subject)
{
    if (subject_container.find(subject) == subject_container.end()) {
        subject_container[subject] = std::make_shared<subject_content>(subject);
        std::cout << "build_subject: " << subject << std::endl;
        return;
    }
    std::cout << "subject: " << subject << "already build" << std::endl;
}

std::shared_ptr<subject_content> observer_container::query_subject(subject_t subject)
{
    auto subject_content_iter = subject_container.find(subject);
    if (subject_content_iter == subject_container.end()) {
        return nullptr;
    }
    return subject_content_iter->second;
}

std::vector<observer_cb_t> observer_container::query_observer(subject_t subject)
{
    auto subject_cb_iter = subject_cb_container.find(subject);
    return (subject_cb_iter == subject_cb_container.end()) ? std::vector<observer_cb_t>() : subject_cb_iter->second;
}
