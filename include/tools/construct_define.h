// description: 构造函数宏定义
// author: zhangzhiyu
// date: 2022.4.19

#ifndef INCLUDE_TOOLS_CONSTRUCT_DEFINE
#define INCLUDE_TOOLS_CONSTRUCT_DEFINE

#define DISALLOW_COPY_AND_ASSIGN(class_name) \
    class_name(const class_name&) = delete; \
    class_name& operator=(const class_name&) = delete

#endif