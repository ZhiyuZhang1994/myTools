/**
 * @brief 所有定时任务放在一个定时器线程中
 * @brief 该工具未限制堆大小，存在队列过大风险
 * @brief 任务在栈上，离开作用域timer变野指针，需要搞到堆上，或构造时返回一个timer_handler句柄用于管理
 * @author http://t.zoukankan.com/warnet-p-10528650.html
 * @date 2022/09/07
 */

#include "timer.h"
#include <utility>

//////////////////////////////////////////////////////////////////////////
// Timer

// 默认构造没有指定任务，因此 heapIndex_ = -1, interval_ = 0
Timer::Timer(TimerManager &manager)
    : manager_(manager), timerType_(CIRCLE), callbackArgs_(nullptr), interval_(0), expires_(0), heapIndex_(-1) {
}

Timer::~Timer() {
    stop();
}

// 主动停止任务
void Timer::stop() {
    if (heapIndex_ != -1) {
        manager_.removeTimer(this);
        heapIndex_ = -1;
    }
}

void Timer::onTimer(std::uint64_t now) {
    if (timerType_ == Timer::CIRCLE) {
        expires_ = interval_ + now;
        manager_.addTimer(this);
    } else {
        heapIndex_ = -1;
    }
    timerCallback_(callbackArgs_);
}

//////////////////////////////////////////////////////////////////////////
// TimerManager

void TimerManager::addTimer(Timer *timer) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    timer->heapIndex_ = heap_.size();
    HeapEntry entry = {timer->expires_, timer};
    heap_.push_back(entry);
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
            if (index > 0 && heap_[index].time < heap_[parent].time)
                upHeap(index);
            else
                downHeap(index);
        }
    }
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
    }
    finish_ = true;
}