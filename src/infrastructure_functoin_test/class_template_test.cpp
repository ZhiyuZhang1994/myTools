/**
 * @brief 类模板用法测试
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-06-05
 */

#include <chrono>
#include <ctime>
#include <iostream>
#include <ratio>
#include <thread>

// 类模板
template<class T1, class T2>
class MyExample {
public:
    bool equal(T1 a, T1 b);

private:
    T2 data;
};

template<class T1, class T2>
bool MyExample<T1, T2>::equal(T1 a, T1 b) {
    return a == b;
}
//     MyExample<int, double> a;

// 类模板全特化：指定模板所有参数的具体类型
template<>
class MyExample<float, int> {
public:
    bool equal(float a, float b);

private:
    int data;
};

bool MyExample<float, int>::equal(float a, float b) {
    std::cout << "全特化" << std::endl;
    return std::abs(a - b) < 10e-3;
}
//     MyExample<float, int> a;

// 类模板偏特化：指定模板部分参数的具体类型
template<class T>
class MyExample<float, T> {
public:
    bool equal(float a, float b);

private:
    T data;
};

template<class T>
bool MyExample<float, T>::equal(float a, float b) {
    std::cout << "偏特化" << std::endl;
    return std::abs(a - b) < 10e-3;
}
// MyExample<float, double> a;


// 成员函数模板：函数模板作为类模板成员
template<class T1, class T2>
class MyExample2 {
public:
    bool equal(T1 a, T1 b);

    template<typename T3>
    void func1(T3 arg) {
        std::cout << arg << std::endl;
    }

private:
    T2 data;
};

template<class T1, class T2>
bool MyExample2<T1, T2>::equal(T1 a, T1 b) {
    return a == b;
}
// MyExample2<int, double> b;

// 类模板的类型模板参数携带默认类型
template<class T1 = int, class T2 = double>
class MyExample3 {
public:
    bool equal(T1 a, T1 b);

private:
    T2 data;
};

template<class T1, class T2>
bool MyExample3<T1, T2>::equal(T1 a, T1 b) {
    return a == b;
}
// MyExample3<> c;

// 类模板的非类型模板参数携带默认值
template<class T1, class T2, std::uint32_t num = 5>
class MyExample4 {
public:
    bool equal(T1 a, T1 b);

private:
    T2 values[num];
};

template<class T1, class T2, std::uint32_t num>
bool MyExample4<T1, T2, num>::equal(T1 a, T1 b) {
    return a == b;
}
    MyExample4<int, double, 6> d;


// 普调函数模板
template<typename T1>
bool equal(T1 a, T1 b) {
    std::cout << "aaa" << std::endl;
    return a == b;
}

// 函数模板全特化
template<>
bool equal(double a, double b) {
    std::cout << "偏特化" << std::endl;
    return std::abs(a - b) < 10e-3;
}

int main() {
    equal(2.1,2.1);
    equal(1,3);
    return 0;
}