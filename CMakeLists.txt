cmake_minimum_required(VERSION 3.10)

project(tohka)

set(VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR})

SET(CXX_FLAGS
        -g
        -Wall
        -O2
        )
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

option(BUILD_EXAMPLES "build examples" ON)
option(BUILD_TEST "build tests" OFF)


string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})
message(${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(tohka)

if (BUILD_EXAMPLES)
   add_subdirectory(examples)
endif ()
