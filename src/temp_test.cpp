/**
 * @brief 显示节点与单元ID编号
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-08-23
 */

// steady_clock example

#include "tools/ticker.h"
#include "tools/timer/timer.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <ratio>
#include <thread>

int main() {
    Timer::create();
    abc::Ticker ticker;
    // ticker.start();
    // for (int i = 0; i < 10; i++) {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 500ms between ticks.
    //     ticker.tick("zzy");
    // }
    // ticker.dumpEveryTickElapseInfo();
    // ticker.dumpTotalTimeElapseInfo();
    return 0;
}