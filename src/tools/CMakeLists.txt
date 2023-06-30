# 收集当前目录中所有源文件的名称，并将列表存储在提供的source_list变量中。
aux_source_directory(. source_list)

# 生成动态库tools
add_library(tools SHARED ${source_list})
