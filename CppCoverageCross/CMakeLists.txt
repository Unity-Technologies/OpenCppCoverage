cmake_minimum_required(VERSION 2.8.12)

set (CMAKE_CXX_STANDARD 14)

add_library(CppCoverageCross
        CoverageData.hpp
        CoverageData.cpp
        CoverageRate.hpp
        CoverageRate.cpp
        CoverageRateComputer.hpp
        CoverageRateComputer.cpp
        CppCoverageException.hpp
        CppCoverageException.cpp
        CppCoverageExport.hpp
        FileCoverage.hpp
        FileCoverage.cpp
        LineCoverage.hpp
        LineCoverage.cpp
        ModuleCoverage.hpp
        ModuleCoverage.cpp)

include_directories(${CMAKE_SOURCE_DIR})
link_directories(${CMAKE_SOURCE_DIR})