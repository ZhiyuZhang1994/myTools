/**
 * @brief 打点计时器工具
 * 功能列表：
 * 1、查询每次打点或者整个打点过程的耗时与描述信息
 * 2、设置输出单位(s、ms)，默认为：ms
 * 3、设置打点信息实时输出或者用户自定义时间输出
 * @author zhangzhiyu
 * @date 2023-05-30
 */

#ifndef INCLUDE_TOOLS_TICKER_H_
#define INCLUDE_TOOLS_TICKER_H_

#include <chrono>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

namespace abc {

class Ticker {
public:
    static const std::uint16_t MAX_TICK_TIMES = 500; // 最大的打点次数
    using TimeStampInfo_t = std::pair<std::chrono::steady_clock::time_point, std::string>;

    enum TickTimeUnit {
        MS, // milliseconds. 默认。
        S, // seconds.
    };

public:
    Ticker(std::string info = "Default ticker name");
    ~Ticker();

    /**
     * @brief 设置输出单位(s、ms)
     */
    void setTickTimeUnit(TickTimeUnit unit);

    /**
     * @brief 设置实时输出
     */
    void setOutputTickInfoInRealTime();

    /**
     * @brief 打印计时全过程对应的耗时数据
     */
    void dumpTotalTimeElapseInfo();

    /**
     * @brief 打印每次打点对应的耗时数据
     */
    void dumpEveryTickElapseInfo();

    /**
     * @brief 获取定时器启动的时间
     * @return steady_clock::time_point 启动时间
     */
    std::chrono::steady_clock::time_point getStartTime() const;

    /**
     * @brief 获取打点计数器的总用时
     * 
     * @return std::uint64_t 总共用时(ms)
     */
    std::uint64_t getTotalTimeElapsed();

    /**
     * @brief 获取当前打点的次数
     * @return std::uint32_t 打点次数
     */
    std::uint32_t getCount() const;

    /**
     * @brief 查询打点index对应的打点时间
     * 
     * @param index 打点次数
     * @return steady_clock::time_point 打点时对应的时间
     */
    std::chrono::steady_clock::time_point getTickTime(std::uint32_t index);

    /**
     * @brief 查询的打点次数是否有效
     * 
     * @param index 打点次数
     * @return true 无效
     * @return false 有效
     */
    bool isInValidIndex(std::uint32_t index);

    /**
     * @brief 启动计时
     */
    void start(std::string msg = "start");

    /**
     * @brief 打点计时接口
     * 
     * @return std::uint64_t 返回上次打点到此次打点经过的时间(ms)
     */
    std::uint64_t tick(std::string msg = "");

    /**
     * @brief 获取花费时间最长的一个打点索引
     * @return std::uint32_t 花费时间最长阶段对应的打点次数
     */
    std::uint32_t getTickMaxElapsedTimeIndex();

    /**
     * @brief 获取花费时间最短的一个打点索引
     * @return std::uint32_t 花费时间最短阶段对应的打点次数
     */
    std::uint32_t getTickMinElapsedTimeIndex();

private:
    /**
     * @brief 获取当前的时间
     * @return steady_clock::time_point 时间对象
     */
    static std::chrono::steady_clock::time_point getCurrTime();

    /**
     * @brief 获取自上一次打点到此次打点经过的时间信息
     * 
     * @param index 此次打点的索引: 从序号1开始
     * @return std::pair<std::uint64_t, std::string> 经过的时间(ms)及描述
     */
    std::pair<std::uint64_t, std::string> getTickElapsedTimeInfo(std::uint32_t index);

private:
    static const std::unordered_map<TickTimeUnit, std::string> UNIT_STRING; // 单位信息

    bool realTime_ = false; // 默认不实时输出，不影响程序运行性能

    std::chrono::steady_clock::time_point startTime_; // 开始计时的时间

    std::chrono::steady_clock::time_point endTime_; // 最后一次计时的时间

    std::uint32_t count_ = 0; // 打点的次数

    std::vector<TimeStampInfo_t> tickInfo_; // 存储每次打点的信息

    std::string tickerInfo_; // 计时器描述

    TickTimeUnit unit_ = TickTimeUnit::MS; // 默认输出单位
};
}

#endif // INCLUDE_TOOLS_TICKER_H_
