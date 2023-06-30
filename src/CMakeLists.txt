# 添加子目录
add_subdirectory(tools)

# 获取当前文件夹下cpp
aux_source_directory(. source_list)

# 生成库文件
add_library(easylogginglib SHARED $<TARGET_OBJECTS:easylogging>)
add_library(tools SHARED $<TARGET_OBJECTS:tool>)

# 生成可执行文件
# VTK 可执行文件
# add_executable(MY_CPP_TOOLS MACOSX_BUNDLE ./vtk/change_layer/set_actor_layer_by_renderer.cpp)
add_executable(MY_CPP_TOOLS MACOSX_BUNDLE ./temp_test.cpp)

# add_executable(MY_CPP_TOOLS MACOSX_BUNDLE ${source_list})
TARGET_COMPILE_OPTIONS(MY_CPP_TOOLS PRIVATE -W -Wall -Wno-unused-value -Wno-unused-parameter)


# 添加链接库
target_link_libraries(MY_CPP_TOOLS PRIVATE
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
  TARGETS MY_CPP_TOOLS
  MODULES ${VTK_LIBRARIES}
)