# 收集当前目录中所有源文件的名称，并将列表存储在提供的source_list变量中。
aux_source_directory(. source_list)

# 生成动态库tools
add_library(tools SHARED ${source_list})


# 给target添加独有的链接库
target_link_libraries(tools PRIVATE
    ${Python3_LIBRARIES}
)

# 给target添加独有的包含目录
TARGET_INCLUDE_DIRECTORIES(tools PRIVATE
    # 头文件路径
    ${Python3_INCLUDE_DIRS}
)
