/**
 * @brief 打点计时器工具
 * @author zhangzhiyu
 * @date 2023-05-30
 */

#include "ticker.h"
#include <chrono>
#include <iostream>

using namespace std::chrono;

Ticker::Ticker(std::string pInfo) {
    startTime_ = getCurrTime();
    endTime_ = startTime_;
    counterTimes_[count_++] = startTime_;
    timerInfo_ = pInfo;
}

Ticker::~Ticker() {
}

void Ticker::start() {
    count_ = 0;
    startTime_ = getCurrTime();
    endTime_ = startTime_;
    counterTimes_[count_] = startTime_;
    count_++;
}

std::uint64_t Ticker::tick() {
    steady_clock::time_point tmpTime = endTime_;
    endTime_ = getCurrTime();
    counterTimes_[count_++] = endTime_;
    return duration_cast<duration<std::uint64_t, std::milli>>(endTime_ - tmpTime).count();
}

steady_clock::time_point Ticker::getStartTime() const {
    return startTime_;
}

std::uint32_t Ticker::getCount() const {
    return count_;
}

steady_clock::time_point Ticker::getCurrTime() {
    steady_clock::time_point t1 = steady_clock::now();
    return t1;
}

steady_clock::time_point Ticker::getTickTime(std::uint32_t index) {
    if (isInValidIndex(index)) {
        return steady_clock::time_point::max();
    }
    return counterTimes_[index];
}

bool Ticker::isInValidIndex(std::uint32_t index) {
    return index > count_ - 1;
}

void Ticker::dumpEveryTickElapseInfo() {
    char info[20];
    for (std::uint32_t i = 0; i < count_; i++) {
        sprintf(info, "Tick %d : ", i + 1);
        std::uint64_t spendTime = getTickElapsedTime(i);
        std::cout << info << spendTime << std::endl;
    }
}

void Ticker::dumpTotalTimeElapseInfo() {
    std::uint64_t spendTime = getTotalTimeElapsed();
    std::cout << "total spend: " << spendTime << " ms;" << std::endl;
}

std::uint64_t Ticker::getTotalTimeElapsed() {
    duration<std::uint64_t, std::milli> milliseconds_span = duration_cast<duration<std::uint64_t, std::milli>>(endTime_ - startTime_);
    return milliseconds_span.count();
}

std::uint64_t Ticker::getTickElapsedTime(std::uint32_t index) {
    if (isInValidIndex(index)) {
        return std::numeric_limits<std::uint64_t>::max();
    }
    duration<std::uint64_t, std::milli> milliseconds_span;
    if (index == 0) {
        milliseconds_span = duration_cast<duration<std::uint64_t, std::milli>>(counterTimes_[index] - startTime_);
    } else {
        milliseconds_span = duration_cast<duration<std::uint64_t, std::milli>>(counterTimes_[index] - counterTimes_[index - 1]);
    }
    return milliseconds_span.count();
}

std::uint32_t Ticker::getTickMaxElapsedTimeIndex() {
    std::uint32_t result = -1;
    std::uint64_t m_MaxTime = 0;
    std::uint64_t tmpTime;
    for (std::uint32_t i = 0; i < count_; i++) {
        tmpTime = getTickElapsedTime(i);
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
        tmpTime = getTickElapsedTime(i);
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
