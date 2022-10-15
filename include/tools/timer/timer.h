/**
 * @brief 所有定时任务放在一个定时器线程中
 * @author http://t.zoukankan.com/warnet-p-10528650.html
 * https://blog.csdn.net/lizhijian21/article/details/83417747?spm=1001.2014.3001.5506
 * @date 2022-09-07
 */

#ifndef INCLUDE_TOOLS_TIMER_TIMER_H
#define INCLUDE_TOOLS_TIMER_TIMER_H

#include <functional>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

class Timer {
public:
    using CallbackType = std::function<void()>;
    using TimeHandler = Timer*;

public:
    static TimeHandler create() { return new Timer; }

    // 停止执行任务: 周期性任务可以停止，一次性任务均不可停止。
    void stop();

    // 周期性任务
    template<typename callback, typename... arguments>
    static TimeHandler loop(uint64_t ms, callback&& func, arguments&&... args) {
        TimeHandler handler = create();
        std::function<void()> task(std::bind(std::forward<callback>(func), std::forward<arguments>(args)...));
        handler->start(ms, task, true);
        return handler;
    }

    // 创建一次性任务
    template<typename callback, typename... arguments>
    void once(callback&& func, arguments&&... args) {
        using namespace std::literals::chrono_literals;
        std::function<void()> task(std::bind(std::forward<callback>(func), std::forward<arguments>(args)...));
        TimeHandler handler = create();
        handler->start(0, task, false);
    }

    // 创建一次性延迟执行任务
    template<typename callback, typename... arguments>
    void onceDelay(uint64_t delayMS, callback&& func, arguments&&... args) {
        using namespace std::literals::chrono_literals;
        std::function<void()> task(std::bind(std::forward<callback>(func), std::forward<arguments>(args)...));
        start(delayMS, task, false);
    }

private:
    Timer() = default;
    void start(uint64_t ms, CallbackType fun, bool loop = false);

    // 到时间了开始执行
    void onTimer(uint64_t now);

private:
    friend class TimerManager;
    bool loop_ = false;
    CallbackType timerCallback_ = nullptr;  // 任务函数
    std::uint64_t interval_ = 0;  // 任务执行周期
    std::uint64_t expires_ = 0;  // 指定任务到期结束时间，用于在最小堆中任务执行顺序排序
    std::int32_t heapIndex_ = -1; // 用于任务索引 -1表示已经不再队列中，即任务执行完成了
};
using TimeHandler = Timer*;

// 定时器管理类：
// 1、管理了定时器线程：该线程处理所有定时任务
// 2、用最小堆管理任务是否到时间
// 3、最高精度未100ms
class TimerManager {
public:
    static TimerManager &instance() {
        static TimerManager _instance;
        return _instance;
    }
    ~TimerManager() {
        stop();
    }

    /**
     * 启动最大精度100ms的定时器
     * @brief 启动timer管理线程
     */
    void start();
    void stop();

private:
    friend class Timer;
    TimerManager() = default;

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
    bool finish_ = false;
};

#endif