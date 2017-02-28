# Minimum CMake required
cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-D__STRICT_ANSI__")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
find_package(Boost)

message(STATUS "making CoverageTool...")

if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(
        C:/Users/nicklas/Downloads/boost_1_63_0
        C:/Users/nicklas/Downloads/boost_1_63_0/boost/tr1/tr1
        ${PROJECT_SOURCE_DIR})

link_libraries(ExporterCross CppCoverageCross
        "/usr/lib/libboost_system.dll.a"
        "/usr/lib/libboost_filesystem.dll.a"
        "/usr/lib/libboost_log.dll.a"
        "/usr/lib/libboost_log_setup.dll.a"
        "/usr/lib/libctemplate.dll.a")

add_executable(CovTool CoverageTool.cpp)