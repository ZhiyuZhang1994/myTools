# 全局头文件包含
include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加子目录
add_subdirectory(a)
add_subdirectory(b)

# 获取当前文件夹下cpp
aux_source_directory(. source_list)

# 生成库文件
add_library(xx SHARED $<TARGET_OBJECTS:ax> $<TARGET_OBJECTS:bx>)

# 生成可执行文件
add_executable(Tutorial_Step1 MACOSX_BUNDLE class_auto_reg_test_main.cpp )

# 添加链接库
# target_link_libraries(Tutorial_Step1 PRIVATE ${VTK_LIBRARIES} xx boost_serialization)
target_link_libraries(Tutorial_Step1 ${Boost_SERIALIZATION_LIBRARY_RELEASE})
# target_link_libraries(Tutorial_Step1 PRIVATE ${VTK_LIBRARIES} xx boost_serialization boost_system)

# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS Tutorial_Step1
  MODULES ${VTK_LIBRARIES}
)