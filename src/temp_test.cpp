/**
 * @brief 显示节点与单元ID编号
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-08-23
 */

// steady_clock example
#include <chrono>
#include <ctime>
#include <iostream>
#include <ratio>
#include "ticker.h"
#include <thread>

int main() {
    Ticker performanceCounter("Refresh Original");
    performanceCounter.start();
    for (int i = 0; i < 10; i++) {
        performanceCounter.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 500ms between ticks.
    }
    performanceCounter.dumpEveryTickElapseInfo();
    performanceCounter.dumpTotalTimeElapseInfo();
    return 0;
}