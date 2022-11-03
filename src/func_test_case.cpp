/**
 * @brief 功能测试
 * @author zhangzhiyu
 * @date 2022-09-16
 */

#include "tools/tassk_class.h"
#include "tools/msg_service.h"
#include "tools/table_driven.h"

#include <utility>
#include <iostream>
#include <signal.h>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <cstring>

template<typename T>
void func2(std::vector<T>* abc) {
    std::cout << "func2" << std::endl;
}

template<typename T>
void func1(std::vector<T>* abc) {
    func2(abc);
}

int main()
{
    std::vector<int> abc{1,2,3};
    func1(&abc);
    getchar();
    return 0;
}