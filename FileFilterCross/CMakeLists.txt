cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 14)

add_library(FileFilterCross
        AmbiguousPathException.cpp
        AmbiguousPathException.hpp
        File.cpp
        File.hpp
        FileFilterException.hpp
        FileFilterExport.hpp
        FileInfo.hpp
        ModuleInfo.hpp
        PathMatcher.cpp
        PathMatcher.hpp
        stdafx.cpp
        stdafx.h
        UnifiedDiffCoverageFilter.cpp
        UnifiedDiffCoverageFilter.hpp
        UnifiedDiffParser.cpp
        UnifiedDiffParser.hpp
        UnifiedDiffParserException.cpp
        UnifiedDiffParserException.hpp
        )

if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")
    include_directories(${PROJECT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})

    target_link_libraries(FileFilterCross
            ${Boost_LIBRARIES}
            )
endif()

include_directories(${CMAKE_SOURCE_DIR})
link_directories(${CMAKE_SOURCE_DIR})