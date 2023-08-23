/**
 * @brief GUI层模块加载框架
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-08-26
 */

#ifndef GUI_GUI_COMMON_DEFINE_H
#define GUI_GUI_COMMON_DEFINE_H

#include <cstdint>

namespace ZZY_TOOLS
{

enum InitStage : std::uint8_t {
    INIT_SELF = 0,  // 初始化自己阶段：完成类内成员构造，变量初始化
    INIT_WITH_OTHERS = 1,  // 初始化周边交互阶段：完成与周边相关的回调函数注册、信息交互(此时能保证周边类在ININ_SELF阶段初始化完成)
    INIT_POST = 2,  // 初始化完成阶段：开启定时器、巡检任务等功能，表示该服务已经正常运行
};

}

#endif // GUI_GUI_COMMON_DEFINE_H
