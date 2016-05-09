cmake_minimum_required(VERSION 3.5)
project(own)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    1.c
    2.c
    fork-wait.c
    setup-argv.c)

add_executable(own ${SOURCE_FILES})