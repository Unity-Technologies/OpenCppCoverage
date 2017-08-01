cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_CXX_STANDARD 14)

add_library(ToolsLinux
        Tool.hpp
        Tool.cpp
        ToolsExport.hpp)


include_directories(${PROJECT_SOURCE_DIR})