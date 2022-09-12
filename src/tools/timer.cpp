/**
 * @brief 所有定时任务放在一个定时器线程中
 * @author http://t.zoukankan.com/warnet-p-10528650.html
 * @date 2022-09-07
 */

#include "tools/timer/timer.h"
#include <utility>
#include <iostream>

///////////////////////////////////Timer///////////////////////////////////////

void Timer::start(uint64_t ms, CallbackType func, bool loop) {
    interval_ = ms;
    timerCallback_ = std::move(func);
    loop_ = loop;
    using namespace std::chrono;
    expires_ = interval_ + duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    TimerManager::instance().addTimer(this);
}

// 主动停止任务
void Timer::stop() {
    if (heapIndex_ != -1) {
        TimerManager::instance().removeTimer(this);
        heapIndex_ = -1;
    }
    delete this;
}

void Timer::onTimer(std::uint64_t now) {
    timerCallback_();
    if (loop_) {
        expires_ = interval_ + now;
        TimerManager::instance().addTimer(this);
    } else {
        stop();
    }
}

/////////////////////////////////////TimerManager/////////////////////////////////////

void TimerManager::addTimer(Timer *timer) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    timer->heapIndex_ = heap_.size();
    HeapEntry entry = {timer->expires_, timer};
    heap_.push_back(entry);
    constexpr std::uint8_t MAX_TASK_NUMBER = 10;
    if (heap_.size() > MAX_TASK_NUMBER) {
        std::cout << "too many tasks in timer" << std::endl;
    }
    upHeap(heap_.size() - 1);
}

void TimerManager::removeTimer(Timer *timer) {
    std::size_t index = timer->heapIndex_;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!heap_.empty() && index < heap_.size()) {
        if (index == heap_.size() - 1) {
            heap_.pop_back();
        } else {
            swapHeap(index, heap_.size() - 1);
            heap_.pop_back();
            std::size_t parent = (index - 1) / 2;
            if (index > 0 && heap_[index].time < heap_[parent].time) {
                upHeap(index);
            } else {
                downHeap(index);
            }
        }
    }
    timer->heapIndex_ = -1;
}

void TimerManager::detectTimers() {
    using namespace std::chrono;
    std::uint64_t now = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    while (!heap_.empty() && heap_[0].time <= now) {
        Timer *timer = heap_[0].timer;
        removeTimer(timer);
        timer->onTimer(now);
    }
}

void TimerManager::upHeap(std::size_t index) {
    std::size_t parent = (index - 1) / 2;
    while (index > 0 && heap_[index].time < heap_[parent].time) {
        swapHeap(index, parent);
        index = parent;
        parent = (index - 1) / 2;
    }
}

void TimerManager::downHeap(std::size_t index) {
    std::size_t child = index * 2 + 1;
    while (child < heap_.size()) {
        std::size_t minChild = (child + 1 == heap_.size() || heap_[child].time < heap_[child + 1].time)
                                   ? child
                                   : child + 1;
        if (heap_[index].time < heap_[minChild].time)
            break;
        swapHeap(index, minChild);
        index = minChild;
        child = index * 2 + 1;
    }
}

void TimerManager::swapHeap(std::size_t index1, std::size_t index2) {
    HeapEntry tmp = heap_[index1];
    heap_[index1] = heap_[index2];
    heap_[index2] = tmp;
    heap_[index1].timer->heapIndex_ = index1;
    heap_[index2].timer->heapIndex_ = index2;
}

void TimerManager::start() {
    finish_ = false;
    thread_ = std::thread([this]() {
        while (!finish_) {
            detectTimers();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
    thread_.detach();
}

void TimerManager::stop() {
    for (auto entry : heap_) {
        removeTimer(entry.timer);
        entry.timer->stop();
    }
    finish_ = true;
}