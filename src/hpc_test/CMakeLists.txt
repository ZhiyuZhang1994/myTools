find_package(MPI REQUIRED)
if (MPI_FOUND)
    include_directories(${MPI_INCLUDE_PATH})
    # 不需要手动设置编译器，find_package(MPI) 会自动设置
    # set(CMAKE_C_COMPILER ${MPI_C_COMPILER})
    # set(CMAKE_CXX_COMPILER ${MPI_CXX_COMPILER})
endif()

# complie flag
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC  -std=c99")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++11")

# 解决C++与C语言混合编译问题
add_definitions(-DMPICH_SKIP_MPICXX)

# 查找 OpenMP 包
find_package(OpenMP REQUIRED)
# 如果找到 OpenMP，将其链接到目标
if(OpenMP_FOUND)
    message(STATUS "OpenMP found, enabling support.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")  # 添加 OpenMP 编译选项
endif()

find_package(OpenBLAS REQUIRED)

# 暂时解决mingw下OpenBLAS_LIBRARIES未定义问题。之后统一编写cmake文件
# 定义宏来根据操作系统选择链接方式
macro(link_openblas target)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        # 在 Ubuntu 上使用 ${OpenBLAS_LIBRARIES}
        target_link_libraries(${target} PRIVATE ${OpenBLAS_LIBRARIES})
        message(STATUS "Linking OpenBLAS on Linux: ${OpenBLAS_LIBRARIES}")
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        # 在 MinGW 上使用 OpenBLAS::OpenBLAS
        target_link_libraries(${target} PRIVATE OpenBLAS::OpenBLAS)
        message(STATUS "Linking OpenBLAS on Windows (MinGW): OpenBLAS::OpenBLAS")
    else()
        message(FATAL_ERROR "Unsupported system: ${CMAKE_SYSTEM_NAME}")
    endif()
endmacro()

add_executable(ex1 app.c)

if (OpenBLAS_FOUND)
    message(STATUS "OpenBLAS found, enabling support.")
    include_directories(${OpenBLAS_INCLUDE_DIRS})
    link_openblas(ex1)
else()
    message(FATAL_ERROR "OpenBLAS not found!")
endif()


set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include_directories(
    "C:/msys2/mingw64/include/petsc"
    "C:/msys2/mingw64/include/petsc/smo"
)

link_directories(
    "C:/msys2/mingw64/bin/"
)

target_link_libraries(ex1
    PUBLIC
        ${MPI_C_LIBRARIES}  # Link MPI libraries
        OpenMP::OpenMP_C
        "C:/msys2/mingw64/bin/libpetsc-smo.dll"
)








# list(APPEND CMAKE_PREFIX_PATH "C:/msys2/mingw64/bin") # 安装路径前缀


# # 获取当前文件夹下cpp
# aux_source_directory(. source_list)

# # 生成库文件：将object库组装成常规库
# add_library(easylogging SHARED $<TARGET_OBJECTS:easylogging_obj>)

# # 生成可执行文件
# # add_executable(testCase MACOSX_BUNDLE ./temp_test.cpp)
# add_executable(testCase ${source_list})

# # 指定target的编译选项
# TARGET_COMPILE_OPTIONS(testCase PRIVATE -W -Wall -Wno-unused-value -Wno-unused-parameter)
# add_compile_definitions(EIGEN_USE_BLAS)

# # 给target添加独有的链接库
# target_link_libraries(testCase PRIVATE
#     ${VTK_LIBRARIES}
#     # ${Boost_SERIALIZATION_LIBRARY_RELEASE}
#     easylogging
#     # tools
#     ${BLAS_LIBRARIES}
# )


# # 给target添加独有的包含目录
# TARGET_INCLUDE_DIRECTORIES(testCase PRIVATE
#     # 头文件路径
#     ${PROJECT_SOURCE_DIR}/test
# )
