#include "test_moudle.h"
#include "tools/notifier_and_observer.h"
#include "tools/declare_operator_funcs.h"
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <unordered_set>
#include <map>
#include <string>
#include <memory>

// 一、测试单例模式
void test_singleton()
{
    A::get_instance(2, 3)->print_a_b();
    std::cout << "--------Split line-------------" << std::endl;
    B::get_instance()->print_empty();
    A::get_instance(2, 3)->print_a_b();
}

// 二、测试类自注册及初始化
void test_auto_reg()
{
    class_repository::get_instance()->active();
}

// 三、测试观察者机制

// 三、结构体大小比较
void test_struct_compare()
{
    direction dir1{1,2,3};
    direction dir2(2,3,4);
    direction dir3{3,4,5};
    if (dir1 < dir2) {
        std::cout << "dir1 < dir2" << std::endl;
    }
}

class A1{
public:
    A1() = default;
    int x = 1;
};

class B1: public A1 {
public:
    B1() : A1() {}
    int y = 2;
};

int main()  {
    std::shared_ptr<A1> x = std::make_shared<A1>();
    std::shared_ptr<B1> y = std::make_shared<B1>();
    y =x;
    test_struct_compare();
    getchar();
}