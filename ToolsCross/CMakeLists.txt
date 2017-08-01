cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_SOURCE_DIR})

add_library(ToolsCross
        ExceptionBase.cpp
        ExceptionBase.hpp
        Log.hpp
        Log.cpp
        ScopedAction.hpp
        ScopedAction.cpp
        ToolsException.hpp
        ToolsExport.hpp
        UniquePath.hpp
        UniquePath.cpp)

link_directories(${CMAKE_SOURCE_DIR})