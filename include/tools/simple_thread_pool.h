/**
 * @brief 1. 线程池由多个线程组成，但是所有线程共享一个线程安全的消息队列
 * @brief 2. 消息队列没有优先级
 * @author https://wangpengcheng.github.io/2019/05/17/cplusplus_theadpool/
 * @author https://blog.csdn.net/caoshangpa/article/details/80374651
 * @date 2022/08/18
 */

#ifndef INCLUDE_TOOLS_THREAD_POOL_H
#define INCLUDE_TOOLS_THREAD_POOL_H

#include "safe_queue.h"

#include <util/log_helper.h>
#include <util/non_copyable.h>

#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

class ThreadPool : protected NonCopyable {
private:
    class ThreadWorker {
    public:
        ThreadWorker(ThreadPool *pool, const std::uint32_t id) : workerId_(id), pool_(pool) {
        }

        void operator()() {
            std::function<void()> func;

            bool dequeued;

            while (!pool_->shutdown_) {
                {
                    // 加锁等待任务入队
                    std::unique_lock<std::mutex> lock(pool_->mutQueue_);
                    if (pool_->queue_.empty()) {
                        pool_->cvPool_.wait(lock);
                    }
                    dequeued = pool_->queue_.dequeue(func);
                }

                if (dequeued) {
                    func();
                }
            }
        }

    private:
        // 唯一id
        std::uint32_t workerId_;
        // 所属线程池
        ThreadPool *pool_;
    };

public:
    explicit ThreadPool(std::string name, const std::uint32_t size = 1)
        : shutdown_(false), threads_(std::vector<std::thread>(size)), name_(std::move(name)) {
        for (std::size_t i = 0; i < threads_.size(); ++i) {
            threads_.at(i) = std::thread(ThreadWorker(this, i));
        }
    }

    ~ThreadPool() {
        shutdown();
    }

    /**
     * TODO: 如何处理已经在任务队列的任务
     * @brief 等待所有正在执行任务结束，然后关闭线程池
     */
    void shutdown() {
        std::cout <<< "thread pool " << name_ << " shutdown" << std::endl;
        shutdown_ = true;
        cvPool_.notify_all();

        for (auto &t : threads_) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    /**
     * @brief 异步提交任务到线程池
     * @tparam F 任务类型
     * @tparam Args 任务类型额外参数类型
     * @param f 任务
     * @param args 任务额外参数
     * @return 任务的future以便获得任务执行结果
     */
    template<typename F, typename... Args>
    auto submit(F &&f, Args &&...args) -> std::future<decltype(f(args...))> {
        // 连接函数和参数定义，特殊函数类型，避免左右值错误
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        std::function<void()> wrapperFunc = [taskPtr]() {
            (*taskPtr)();
        };
        queue_.enqueue(wrapperFunc);

        cvPool_.notify_one();
        return taskPtr->get_future();
    }

    /**
     * 全局的默认线程池实例
     * @return
     */
    static ThreadPool &instance() {
        static ThreadPool pool_("main", 4);
        return pool_;
    }

private:
    // 线程池是否关闭
    bool shutdown_;
    // 任务队列
    SafeQueue<std::function<void()>> queue_;
    // 工作线程队列
    std::vector<std::thread> threads_;
    // 任务队列互斥量
    std::mutex mutQueue_;
    // 线程环境锁
    std::condition_variable cvPool_;
    std::string name_;
};

#endif //INCLUDE_TOOLS_THREAD_POOL_H
