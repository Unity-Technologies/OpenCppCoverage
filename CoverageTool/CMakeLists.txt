# Minimum CMake required
cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-D__STRICT_ANSI__")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS filesystem log)

message(STATUS "making CoverageTool...")
link_libraries(
        Exporter
        CppCoverageCross
        FileFilterCross
        ToolsCross
        ToolsLinux
        ctemplate
        pthread)
link_directories(
        /home/nicklas/Downloads/ctemplate-ctemplate-2.3/
        ${Boost_LIBRARY_DIRS})

if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")
    include_directories(${PROJECT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})

    add_executable(CovTool CoverageTool.cpp)

    target_link_libraries(CovTool
            ${Boost_LIBRARIES}
            )
endif()
