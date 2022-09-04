// description: 测试类
// author: zhangzhiyu
// date: 2022.4.19

#include "./tools/Singleton.h"
#include "./tools/construct_define.h"
#include "./tools/class_register.h"
#include "./tools/declare_operator_funcs.h"
#include <iostream>

// 一、测试单例模式A
class A
{
    SINGLETON_CLASS(A);

public:
    void print_a_b() { std::cout << a << b << std::endl;}

private:
    A(int x, int y) : a(x), b(y) {}

private:
    int a;
    int b;
};

// 单例测试类B
class B
{
    SINGLETON_CLASS(B);

public:
    void print_empty() { std::cout << "empty"<< std::endl;}

private:
    B() = default;
};

// 二、测试类自注册及初始化
// 类自动注册测试类C
class C : public base_class
{
public:
    SINGLETON_CLASS(A);

    C(int x, int y) : base_class(), a(x), b(y) {}

    virtual void initialize() override
    {
        std::cout << "class C initialize!" << std::endl;
    }

private:
    int a;
    int b;
    DISALLOW_COPY_AND_ASSIGN(C);
};

// 此处的0x123为自定义的用于表示类C的ID，正式代码中可以用类名hash一个
CLASS_REGISTER(C, 0x123, 1, 2);


// 三、结构体比较函数定义
struct direction
{
    direction(int x, int y, int z) :x_(x), y_(y), z_(z) {}
    int x_;
    int y_;
    int z_;
};
DECLARE_ALL_OPERATOR_FUNCS(direction, x_, y_, z_);

