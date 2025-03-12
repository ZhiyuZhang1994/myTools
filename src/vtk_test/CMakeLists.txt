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
    FiltersModeling

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
if (POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
# find_package(Boost COMPONENTS serialization REQUIRED)
list(APPEND CMAKE_PREFIX_PATH "C:/msys2/mingw64/bin") # 安装路径前缀
list(APPEND CMAKE_PREFIX_PATH "D:/Program Files/Python311/Lib") # 安装路径前缀
# list(APPEND CMAKE_PREFIX_PATH "D:/Program Files/Python311/Lib/site-packages/pybind11/share/cmake/pybind11") # 安装路径前缀
# Find Python
find_package(Python3 COMPONENTS Development REQUIRED)

# 查找 OpenMP
find_package(OpenMP REQUIRED)
find_package(BLAS REQUIRED)

# 添加子目录
# add_subdirectory(tools)


# 获取当前文件夹下cpp
aux_source_directory(. source_list)

# 生成库文件：将object库组装成常规库
add_library(easylogging SHARED $<TARGET_OBJECTS:easylogging_obj>)

# 生成可执行文件
# add_executable(testCase MACOSX_BUNDLE ./temp_test.cpp)
add_executable(testCase ${source_list})

# 指定target的编译选项
TARGET_COMPILE_OPTIONS(testCase PRIVATE -W -Wall -Wno-unused-value -Wno-unused-parameter)
add_compile_definitions(EIGEN_USE_BLAS)

# 给target添加独有的链接库
target_link_libraries(testCase PRIVATE
    ${VTK_LIBRARIES}
    # ${Boost_SERIALIZATION_LIBRARY_RELEASE}
    easylogging
    # tools
    ${BLAS_LIBRARIES}
)

if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP_CXX_FOUND: ${OpenMP_CXX_FOUND}")
    target_link_libraries(testCase PRIVATE OpenMP::OpenMP_CXX)
endif()

# 给target添加独有的包含目录
TARGET_INCLUDE_DIRECTORIES(testCase PRIVATE
    # 头文件路径
    ${PROJECT_SOURCE_DIR}/third_party/easylogging/include
    ${PROJECT_SOURCE_DIR}/include/3rd/eigen3
    ${PROJECT_SOURCE_DIR}/include/3rd
    ${PROJECT_SOURCE_DIR}/test
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
