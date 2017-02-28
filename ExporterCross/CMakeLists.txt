# Minimum CMake required
cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-D__STRICT_ANSI__")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
find_package(Boost)

message(STATUS "start running cmake...")

if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(
        C:/Users/nicklas/Downloads/boost_1_63_0
        C:/Users/nicklas/Downloads/boost_1_63_0/boost/tr1/tr1
        C:/Users/nicklas/Downloads/ctemplate-2.3/ctemplate-2.3/src
        ${PROJECT_SOURCE_DIR})

add_library(ExporterCross
        Html/HtmlFolderStructure.cpp
        Html/HtmlFileCoverageExporter.cpp
        Html/HtmlFile.cpp
        Html/HtmlExporter.cpp
        Html/TemplateHtmlExporter.cpp
        Html/CTemplate.hpp)

link_libraries("/usr/lib/")

target_link_libraries(ExporterCross CppCoverageCross ToolsCross)