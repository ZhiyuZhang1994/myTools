# 添加子目录
add_subdirectory(tools)

# 查找需要的第三方库
find_package(VTK COMPONENTS
    CommonColor
    CommonCore
    CommonDataModel
    FiltersPoints
    FiltersTexture
    FiltersCore
    FiltersExtraction
    FiltersGeneral
    FiltersGeometry
    FiltersSources
    InteractionStyle
    InteractionImage

    RenderingContextOpenGL2
    RenderingCore
    RenderingFreeType
    RenderingGL2PSOpenGL2
    RenderingOpenGL2
    IOImage
    GUISupportQt
    RenderingQt
    RenderingAnnotation

    IOGeometry
    IOLegacy
    IOPLY
    IOXML
    CommonComputationalGeometry
    CommonMath
    CommonTransforms
    ImagingSources
)
if (NOT VTK_FOUND)
  message(FATAL_ERROR "project MY_CPP_TOOLS: Unable to find the VTK build folder.")
endif()

# 查找外部的boost库
find_package(Boost 1.67 COMPONENTS serialization REQUIRED)

# 获取当前文件夹下cpp
aux_source_directory(. source_list)

# 生成库文件：将object库组装成常规库
add_library(easylogging SHARED $<TARGET_OBJECTS:easylogging_obj>)

# 生成可执行文件
# add_executable(testCase MACOSX_BUNDLE ./temp_test.cpp)
add_executable(testCase ${source_list})

# 指定target的编译选项
TARGET_COMPILE_OPTIONS(testCase PRIVATE -W -Wall -Wno-unused-value -Wno-unused-parameter)

# 给target添加独有的链接库
target_link_libraries(testCase PRIVATE
    ${VTK_LIBRARIES}
    ${Boost_SERIALIZATION_LIBRARY_RELEASE}
    easylogging
    tools
)

# 给target添加独有的包含目录
TARGET_INCLUDE_DIRECTORIES(testCase PRIVATE
    # 头文件路径
)

# 获取target的指定属性到变量中
get_target_property(result testCase LINK_DIRECTORIES)
# 打印target的指定属性
message(STATUS "testCase link direcetory: ${result}")


ADD_CUSTOM_COMMAND(
    OUTPUT ${EXECUTABLE_OUTPUT_PATH}/libeasylogging.dll
    COMMAND ${CMAKE_COMMAND} copy ${LIBRARY_OUTPUT_PATH}/libeasylogging.dll ${EXECUTABLE_OUTPUT_PATH}/libeasylogging.dll
        DEPENDS ${LIBRARY_OUTPUT_PATH}/libeasylogging.dll
        COMMENT "for test"
)

# 自定义命令
ADD_CUSTOM_COMMAND(
    TARGET testCase POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/README.md ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/README.md
    COMMENT "copy file for test"
)

# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS testCase
  MODULES ${VTK_LIBRARIES}
)
