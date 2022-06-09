cmake_minimum_required(VERSION 3.10)

project(tohka)

set(VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

link_directories(/usr/local/lib)
SET(CXX_FLAGS
        -g
        -Wall
        #        -O3
        )
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

option(BUILD_EXAMPLES "build examples" ON)
option(BUILD_TEST "build tests" ON)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
include(utils)
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

#configure_file(
#        "${PROJECT_SOURCE_DIR}/config.h.in"
#        "${PROJECT_SOURCE_DIR}/config.h"
#)

message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})
message(${PROJECT_SOURCE_DIR})


check_header("sys/poll.h")

check_function("getpid" "unistd.h")


include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(tohka)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()
#add_subdirectory(tests)
if (BUILD_TESTS)
    add_subdirectory(tests)
endif ()

add_executable(test test.cc)
target_link_libraries(test tohka)
