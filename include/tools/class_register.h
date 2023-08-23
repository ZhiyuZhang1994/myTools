// description: 相同类的自注册框架
// author: zhangzhiyu
// date: 2022.4.20

#ifndef INCLUDE_TOOLS_CLASS_REGISTER
#define INCLUDE_TOOLS_CLASS_REGISTER

#include "tools/Singleton.h"
#include "tools/construct_define.h"
#include "tools/gui_common_define.h"
#include <unordered_map>
#include <memory>
#include <mutex>

namespace ZZY_TOOLS
{

using class_id_t = std::uint32_t;

// 基类
class base_class
{
public:
    base_class() = default;
    virtual void init(InitStage stage) {};
};

using class_reg_func = std::shared_ptr<base_class>(*)();

// 存储所有注册的类
class class_repository {
public:
    SINGLETON_CLASS(class_repository);

    // 注册类
    void sign(class_id_t class_id, class_reg_func reg_func) {
        {
            std::unique_lock<std::mutex> lock(class_reg_func_mutex_);
            if (class_reg_func_container.find(class_id) == class_reg_func_container.end()) {
                class_reg_func_container[class_id] = reg_func;
                std::cout << "reg class_id: " << class_id << std::endl;
            }
        }
        std::cout << "class_id: " << class_id << " already signed!" << std::endl;
    }

    // 构造各类并初始化
    void init(InitStage stage) {
        if (stage == InitStage::INIT_SELF) {
            if (activated_) {
                std::cout << "all the services are activated!" << std::endl;
                return;
            }
            std::unique_lock<std::mutex> lock(class_reg_func_mutex_);
            for (auto &each : class_reg_func_container) {
                std::shared_ptr<base_class> class_ptr = each.second();
                {
                    std::unique_lock<std::mutex> lock(class_ptr_mutex_);
                    class_ptr_container[each.first] = class_ptr;
                }
            }
            activated_ = true;
        }
        {
            std::unique_lock<std::mutex> lock(class_ptr_mutex_);
            for (auto &each : class_ptr_container) {
                each.second->init(stage);
            }
        }
    }

    std::shared_ptr<base_class> get_class_ptr(class_id_t class_id) {
        std::unique_lock<std::mutex> lock(class_ptr_mutex_);
        auto iter = class_ptr_container.find(class_id);
        return (iter == class_ptr_container.end()) ? nullptr : iter->second;
    }

private:
    class_repository() = default;

private:
    std::unordered_map<class_id_t, class_reg_func> class_reg_func_container;
    std::mutex class_reg_func_mutex_;
    std::unordered_map<class_id_t, std::shared_ptr<base_class>> class_ptr_container;
    std::mutex class_ptr_mutex_;
    bool activated_ = false;
};


// 类自注册工具类
class class_register
{
public:
    class_register(class_id_t class_id, class_reg_func reg_func)
    {
        class_repository::get_instance()->sign(class_id, reg_func);
    }

    DISALLOW_COPY_AND_ASSIGN(class_register);
};

// 类自注册宏
#define CLASS_REGISTER(class_name, class_id, ...)                            \
    static std::shared_ptr<base_class> create_##class_name##_register_func() \
    {                                                                        \
        return std::make_shared<class_name>(__VA_ARGS__);                    \
    }                                                                        \
    static class_register _class_reg_##class_name(class_id, create_##class_name##_register_func)


#define DECLARE_CLASS_PTR_TYPE(class_name) \
    using class_name##Ptr = std::shared_ptr<class_name>

#define GET_CLASS_PTR(class_name, class_id) \
    std::dynamic_pointer_cast<class_name>(class_repository::get_instance()->get_class_ptr(class_id))

}

#endif // INCLUDE_TOOLS_CLASS_REGISTER