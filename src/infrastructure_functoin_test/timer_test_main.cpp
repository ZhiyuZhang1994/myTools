
#include "tools/timer/timer.h"
#include <iostream>
#include <utility>

void func1(int x) {
    std::cout<< x << std::endl;
}

void func2() {
    std::cout<< "111" << std::endl;
}
int main() {
    TimerManager::instance().start();
    TimeHandler handler = Timer::loop(100, func1, 5);
    Timer::loop(200, func2);

    getchar();
    TimerManager::instance().stop();
}