# 全局头文件包含
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party)

# 添加子目录
add_subdirectory(tools)

# 获取当前文件夹下cpp
aux_source_directory(. source_list)

# 生成库文件
add_library(easylogginglib SHARED $<TARGET_OBJECTS:easylogging>)
add_library(tools SHARED $<TARGET_OBJECTS:tool>)

# 生成可执行文件
# VTK 可执行文件
add_executable(Tutorial_Step1 MACOSX_BUNDLE ./vtk/composed_actor/pixel_size_of_actor.cpp)

# add_executable(Tutorial_Step1 MACOSX_BUNDLE ./temp_test.cpp)


# 添加链接库
target_link_libraries(Tutorial_Step1 PRIVATE
    ${VTK_LIBRARIES}
    ${Boost_SERIALIZATION_LIBRARY_RELEASE}
    easylogginglib
    tools
)

INSTALL(TARGETS easylogginglib
    RUNTIME DESTINATION ${EXECUTABLE_OUTPUT_PATH}
)

ADD_CUSTOM_COMMAND(
    OUTPUT ${EXECUTABLE_OUTPUT_PATH}/libeasylogginglib.dll
    COMMAND ${CMAKE_COMMAND} copy ${LIBRARY_OUTPUT_PATH}/libeasylogginglib.dll ${EXECUTABLE_OUTPUT_PATH}/libeasylogginglib.dll
        DEPENDS ${LIBRARY_OUTPUT_PATH}/libeasylogginglib.dll
        COMMENT "zzy"
)

# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS Tutorial_Step1
  MODULES ${VTK_LIBRARIES}
)