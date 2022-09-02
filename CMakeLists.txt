cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(Tutorial_Step1)


find_package(VTK COMPONENTS 
CommonColor
CommonCore
CommonDataModel
FiltersCore
FiltersExtraction
FiltersGeneral
FiltersGeometry
FiltersSources
InteractionStyle
RenderingContextOpenGL2
RenderingCore
RenderingFreeType
RenderingGL2PSOpenGL2
RenderingOpenGL2
CommonColor
CommonCore
FiltersSources
InteractionStyle
RenderingContextOpenGL2
RenderingCore
RenderingFreeType
RenderingGL2PSOpenGL2
RenderingOpenGL2
GUISupportQt
RenderingQt
RenderingAnnotation
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "Tutorial_Step1: Unable to find the VTK build folder.")
endif()



set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin/)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib) 

FIND_PACKAGE(Boost 1.67 REQUIRED)
set(CMAKE_CXX_FLAGS  "${CMAKE_C_FLAGS}-g -std=c++11") 
# add_subdirectory(third_party)
add_subdirectory(src)

add_definitions(-DELPP_NO_DEFAULT_LOG_FILE)



# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")

