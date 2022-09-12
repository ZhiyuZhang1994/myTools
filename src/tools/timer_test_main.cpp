
#include "tools/timer/timer.h"
#include <iostream>
#include <utility>

void func1(void* ) {
    std::cout<< "1" << std::endl;
}
int main() {
    Timer timer(TimerManager::instance());
    TimerManager::instance().start();

    timer.start(func1, std::chrono::duration<int64_t, std::milli>(100));
    getchar();

}