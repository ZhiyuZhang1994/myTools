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
void signal_callback(int x) {
    std::cout << "signal test" << std::endl;
}

class A {
public:
    void func() {
        std::cout << "A::func" << std::endl;
    }
    void func2() {
        std::cout << "A::func2" << std::endl;
    }

private:
    int a_ = 0;
};

#define ERR_T std::uint32_t

#define FEM_ERROR_SUCCESS 0;

int main()
{
    ERR_T abc = 1;
    if (abc == FEM_ERROR_SUCCESS) {
        std::cout << "FEM_ERROR_SUCCESS" << std::endl;
    }

    double f1 = std::nan("NaN");
    std::array<double, 6> value_{f1};
    value_.fill(f1);
    double x = f1 * f1 + 1;
    double y = f1/2;

    std::cout << "f1: " << f1 << " x: " << x << " y: " << y << std::endl;


    std::cout << "SSSSSSSSSSSSSSSSSSSSSSSSSSS" << std::endl;



    signal(SIGSEGV, signal_callback);
    std::vector<int> arr{1, 2, 3};
    std::cout << arr[0] << std::endl;
    A* a = nullptr;
    a->func();
    int *b;
    *b = 2;
    getchar();
    return 0;
}