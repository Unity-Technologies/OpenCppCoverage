cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_CXX_STANDARD 14)

add_library(ToolsCross
        ExceptionBase.cpp
        ExceptionBase.hpp
        Log.hpp
        Log.cpp
        ScopedAction.hpp
        ScopedAction.cpp
        Tool.cpp
        Tool.hpp
        ToolsException.hpp
        ToolsExport.hpp
        UniquePath.hpp
        UniquePath.cpp)