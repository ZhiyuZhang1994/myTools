/**
 * @brief 打点计时器工具
 * @author zhangzhiyu
 * @date 2023-05-30
 */

#include "tools/ticker.h"
#include <chrono>
#include <iostream>

namespace abc {

const std::unordered_map<Ticker::TickTimeUnit, std::string> Ticker::UNIT_STRING {
    {TickTimeUnit::MS, "ms"},
    {TickTimeUnit::S, "s"},
};

Ticker::Ticker(std::string pInfo) : tickInfo_(MAX_TICK_TIMES) {
    startTime_ = getCurrTime();
    endTime_ = startTime_;
    tickInfo_[count_++] = std::make_pair(startTime_, "Start");
    tickerInfo_ = pInfo;
}

Ticker::~Ticker() {
}

void Ticker::setTickTimeUnit(TickTimeUnit unit) {
    unit_ = unit;
}

void Ticker::setOutputTickInfoInRealTime() {
    realTime_ = true;
}

void Ticker::start(std::string msg) {
    count_ = 0;
    startTime_ = getCurrTime();
    endTime_ = startTime_;
    tickInfo_[count_] = std::make_pair(startTime_, msg);
    count_++;
}

std::uint64_t Ticker::tick(std::string msg) {
    std::chrono::steady_clock::time_point tmpTime = endTime_;
    endTime_ = getCurrTime();
    std::uint64_t spendTime = std::chrono::duration_cast<std::chrono::duration<std::uint64_t, std::milli>>(endTime_ - tmpTime).count();
    spendTime = (unit_ == TickTimeUnit::S) ? spendTime / 1000 : spendTime;

    if (realTime_) {
        std::cout << "Tick " << count_ << ": time:   ";
        std::cout << spendTime << UNIT_STRING.at(unit_) << ";   msg: " << msg << std::endl;
    }
    tickInfo_[count_++] = std::make_pair(endTime_, msg);
    return spendTime;
}

std::chrono::steady_clock::time_point Ticker::getStartTime() const {
    return startTime_;
}

std::uint32_t Ticker::getCount() const {
    return count_;
}

std::chrono::steady_clock::time_point Ticker::getCurrTime() {
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    return t1;
}

std::chrono::steady_clock::time_point Ticker::getTickTime(std::uint32_t index) {
    if (isInValidIndex(index)) {
        return std::chrono::steady_clock::time_point::max();
    }
    return tickInfo_[index].first;
}

bool Ticker::isInValidIndex(std::uint32_t index) {
    return index > count_ - 1;
}

void Ticker::dumpEveryTickElapseInfo() {
    std::cout << "Ticker: " << tickerInfo_ << std::endl;
    for(std::uint32_t i = 1; i < count_; i++) {
        std::cout << "Tick " << i << ": time:   ";
        auto tickTimeInfo  = getTickElapsedTimeInfo(i);
        std::cout << tickTimeInfo.first << UNIT_STRING.at(unit_) << ";   msg: " << tickTimeInfo.second << std::endl;
    }
}

void Ticker::dumpTotalTimeElapseInfo() {
    std::uint64_t spendTime = getTotalTimeElapsed();
    std::cout << "Ticker: " << tickerInfo_ << std::endl;
    std::cout << "Total spend: " << spendTime << UNIT_STRING.at(unit_) << ";" << std::endl;
}

std::uint64_t Ticker::getTotalTimeElapsed() {
    std::uint64_t spendTime =
        std::chrono::duration_cast<std::chrono::duration<std::uint64_t, std::milli>>(endTime_ - startTime_).count();
    return (unit_ == TickTimeUnit::S) ? spendTime / 1000 : spendTime;
}

std::pair<std::uint64_t, std::string> Ticker::getTickElapsedTimeInfo(std::uint32_t index) {
    if (isInValidIndex(index)) {
        return std::make_pair(std::numeric_limits<std::uint64_t>::max(), "invalid index");
    }
    std::pair<std::uint64_t, std::string> res;
    if (index == 0) {
        res = std::make_pair(0, "start");
    } else {
        auto time = std::chrono::duration_cast<std::chrono::duration<std::uint64_t, std::milli>>(tickInfo_[index].first -
            tickInfo_[index - 1].first).count();
        if (unit_ == TickTimeUnit::S) {
            time = time / 1000;
        }
        std::string msg = tickInfo_[index].second;
        res = std::make_pair(time, msg);
    }
    return res;
}

std::uint32_t Ticker::getTickMaxElapsedTimeIndex() {
    std::uint32_t result = -1;
    std::uint64_t m_MaxTime = 0;
    std::uint64_t tmpTime;
    for (std::uint32_t i = 0; i < count_; i++) {
        tmpTime = getTickElapsedTimeInfo(i).first;
        if (tmpTime == std::numeric_limits<std::uint64_t>::max()) { // 时间无穷大则说明ID有问题，此次不计入统计
            continue;
        }
        if (tmpTime > m_MaxTime) {
            m_MaxTime = tmpTime;
            result = i;
        }
    }
    return result;
}

std::uint32_t Ticker::getTickMinElapsedTimeIndex() {
    std::uint32_t result = -1;
    std::uint64_t m_MinTime = std::numeric_limits<std::uint64_t>::max();
    std::uint64_t tmpTime;
    for (std::uint32_t i = 0; i < count_; i++) {
        tmpTime = getTickElapsedTimeInfo(i).first;
        if (tmpTime == std::numeric_limits<std::uint64_t>::max()) { // 时间无穷大则说明ID有问题，此次不计入统计
            continue;
        }
        if (tmpTime < m_MinTime) {
            m_MinTime = tmpTime;
            result = i;
        }
    }
    return result;
}

}