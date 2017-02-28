# Minimum CMake required
cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-D__STRICT_ANSI__")

include_directories(
        ${PROJECT_SOURCE_DIR})

add_library(ToolsCross
        Tool.cpp)