# 指定运行此配置文件所需的 CMake 的最低版本
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# 指定项目的名称
project(MY_CPP_TOOLS)

# 指定编译选项
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -std=c++11 -fPIC")
# 展示所有编译单元所执行的指令 包括 Entering directory Leaving directory
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
set(CMAKE_CXX_COMPILER "g++")
# 启用颜色输出，默认为ON
SET(CMAKE_COLOR_MAKEFILE ON)
# 把编译选项设置为了C++ 11
set(CMAKE_CXX_STANDARD 17)
# 构建中展示更详细的输出
SET(CMAKE_VERBOSE_MAKEFILE ON)

# 默认的构建类型
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

# 指定生成文件路径
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 全局头文件包含
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/third_party/easylogging/include
    ${PROJECT_SOURCE_DIR}/include/pybind11
    # ${Boost_INCLUDE_DIRS}
)

# 全局链接路径包含
link_directories(
    # 写第三方动态库文件存放的路径
)

# 添加要编译的其他文件
add_subdirectory(third_party)
add_subdirectory(src)


# 第三方库用的编译参数
add_definitions(-DELPP_NO_DEFAULT_LOG_FILE)

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
