/**
 * @brief 表驱动：用于降低圈复杂度
 * @brief 对应博客：https://blog.csdn.net/qq_33726635/article/details/115560756
 * @author zhangzhiyu
 * @date 2022.9.17
 */

/* 用法：
    // 简单，推荐使用
    table_driven<int> table1{
        {1, func1},  // 使用默认func参数，调用无参函数
        {2, [&]() {func3(val1,val2);}}  // 使用lambda表达式作为无参形式，实际调用有参函数
    };
    table1.handle_key(condition);

    // 复杂，不推荐使用
    table_driven<int, std::function<void(int, double)>> table2{
        {1, func3},  // 主动修改模板func参数为有参函数
        {2, func4}
    };
    table2.handle_key(condition, val1, val2);
*/

#ifndef INCLUDE_TOOLS_TABLE_DRIVEN_H
#define INCLUDE_TOOLS_TABLE_DRIVEN_H

#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <utility>

template<class key, class func = std::function<void()>, class tbMap = std::map<key, func>>
class TableDriven {
public:
    using valueType = typename tbMap::value_type;
    TableDriven() = default;
    ~TableDriven() = default;

public:
    TableDriven(std::initializer_list<valueType> il) : tableDrivenMap_(il) {}

    void insert(const key& firstValue, func&& secondValue) {
        tableDrivenMap_[firstValue] = std::move(secondValue);
    }

public:
    template<typename... Args>
    bool handleKey(const key& firstValue, Args... args) {
        auto iter = tableDrivenMap_.find(firstValue);
        if (iter != tableDrivenMap_.end()) {
            iter->second(args...);
            return true;
        }
        return false;
    }

private:
    tbMap tableDrivenMap_;
};
#endif
