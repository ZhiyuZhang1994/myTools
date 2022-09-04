// description: 相同类的自注册框架
// author: zhangzhiyu
// date: 2022.4.20

#ifndef INCLUDE_TOOLS_CLASS_REGISTER
#define INCLUDE_TOOLS_CLASS_REGISTER

#include "./Singleton.h"
#include "./construct_define.h"
#include <unordered_map>
#include <memory>

// 基类
class base_class
{
public:
    base_class() = default;
    virtual void initialize() = 0;
};

using class_reg_func = std::shared_ptr<base_class>(*)();

// 存储所有注册的类
class class_repository
{
public:
    using class_id_t = uint16_t;

public:
    SINGLETON_CLASS(class_repository);

    // 注册类
    void sign(class_id_t class_id, class_reg_func reg_func)
    {
        if (class_reg_func_container.find(class_id) == class_reg_func_container.end())
        {
            class_reg_func_container[class_id] = reg_func;
            std::cout << "reg class_id: " << class_id << std::endl;
        }
        std::cout << "class_id: " << class_id << " already signed!" << std::endl;
    }

    // 激活各类并初始化
    void active()
    {
        for (auto &each : class_reg_func_container)
        {
            std::shared_ptr<base_class> class_ptr = each.second();
            class_ptr->initialize();
            class_ptr_container[each.first] = class_ptr;
        }
    }

private:
    class_repository() = default;

private:
    std::unordered_map<class_id_t, class_reg_func> class_reg_func_container;
    std::unordered_map<class_id_t, std::shared_ptr<base_class>> class_ptr_container;
};


// 类自注册工具类
class class_register
{
public:
    class_register(uint16_t class_id, class_reg_func reg_func)
    {
        class_repository::get_instance()->sign(class_id, reg_func);
    }

    DISALLOW_COPY_AND_ASSIGN(class_register);
};

// 类自注册宏
#define CLASS_REGISTER(class_name, class_id, a, b)                           \
    static std::shared_ptr<base_class> create_##class_name##_register_func() \
    {                                                                        \
        return std::make_shared<class_name>(a, b);                           \
    }                                                                        \
    static class_register _class_reg_##class_name(class_id, create_##class_name##_register_func)

#endif