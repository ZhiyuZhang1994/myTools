/**
 * @brief 所有定时任务放在一个定时器线程中
 * @brief 该工具未限制堆大小，存在队列过大风险
 * @author http://t.zoukankan.com/warnet-p-10528650.html
 * @date 2022/09/07
 */

#ifndef INCLUDE_TOOLS_TIMER_TIMER_H
#define INCLUDE_TOOLS_TIMER_TIMER_H

#include <functional>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

class TimerManager;

// 定时任务类：
// 1、表示一个定时任务：任务编写是该类型：std::function<void(void *)>
// 2、三种任务类型：一次性、周期性、一次延期执行性
class Timer {
public:
    enum TimerType {
        ONCE,  // 一次性任务
        CIRCLE,  // 周期性任务
        TIMEOUT  // 延迟执行一次的任务
    };
    using CallbackType = std::function<void(void *)>;

    explicit Timer(TimerManager &manager);
    ~Timer();

    // 创建周期性任务
    template<typename Rep, typename Period>
    void start(CallbackType fun, const std::chrono::duration<Rep, Period> &interval, void *args = nullptr, TimerType timeType = CIRCLE);

    // 创建一次性任务
    inline void runOnce(CallbackType fun, void *args = nullptr) {
        using namespace std::literals::chrono_literals;
        start(std::move(fun), 0s, args, ONCE);
    }

    // 创建一次性延迟执行任务
    template<typename Rep, typename Period>
    inline void Timeout(CallbackType fun, const std::chrono::duration<Rep, Period> &timeout, void *args = nullptr) {
        using namespace std::literals::chrono_literals;
        start(std::move(fun), timeout, args, TIMEOUT);
    }

    // 停止执行任务
    void stop();

private:
    // 到时间了开始执行
    void onTimer(uint64_t now);

private:
    friend class TimerManager;
    TimerManager &manager_;  // 定时器管理类引用
    TimerType timerType_;  // 任务类型
    CallbackType timerCallback_;  // 任务函数
    void *callbackArgs_;  // 任务参数
    std::uint64_t interval_;  // 任务执行周期
    std::uint64_t expires_;  // 指定任务到期结束时间，用于在最小堆中任务执行顺序排序
    std::int32_t heapIndex_; // 用于任务索引 -1表示已经不再队列中，即任务执行完成了
};

// 定时器管理类：
// 1、管理了定时器线程：该线程处理所有定时任务
// 2、用最小堆管理任务是否到时间
// 3、最高精度未100ms
class TimerManager {
public:
    TimerManager() : finish_(false) {}
    ~TimerManager() {
        stop();
    }

    /**
     * 启动最大精度100ms的定时器
     * @brief 启动timer管理线程
     */
    void start();
    void stop();

public:
    /**
     * @brief 全局实例
     * @return TimerManager全局实例引用
     */
    static TimerManager &instance() {
        static TimerManager _instance;
        return _instance;
    }

private:
    friend class Timer;

    // 向最小堆任务队列中添加定时任务
    void addTimer(Timer *timer);
    // 在最小堆任务队列中删除定时任务
    void removeTimer(Timer *timer);

    // 查看堆顶元素是否到时，到时则执行对应任务
    void detectTimers();

    // 最小堆算法：
    // 上筛：将指定元素与之前元素比较，小则上移
    void upHeap(size_t index);
    // 下筛：将指定元素与之后元素比较，大则下移
    void downHeap(size_t index);
    // 交换指定索引的两个元素
    void swapHeap(size_t, size_t index2);

private:
    // 最小堆中元素结构
    struct HeapEntry {
        std::uint64_t time;
        Timer *timer;
    };
    // 最小堆：用vector实现，[0]为堆顶
    std::vector<HeapEntry> heap_;
    // 定时任务线程
    std::thread thread_;
    // 自旋锁，可重入
    std::recursive_mutex mutex_;
    // 是否停止定时任务
    bool finish_;
};

/**
 * 启动定时器
 * @param fun
 * @param interval 如果是CYCLE表示间隔时间;如果是其他则表示延迟启动时间
 * @param args
 * @param timeType
 */
template<typename Rep, typename Period>
void Timer::start(CallbackType fun, const std::chrono::duration<Rep, Period> &interval, void *args, TimerType timeType) {
    using namespace std::chrono;

    stop();
    interval_ = duration_cast<milliseconds>(interval).count();
    timerCallback_ = std::move(fun);
    callbackArgs_ = args;
    timerType_ = timeType;
    this->expires_ = this->interval_ + duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    manager_.addTimer(this);
}

#endif