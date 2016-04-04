cmake_minimum_required(VERSION 3.3)
project(TDIU16)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    README.md 1.c)

add_executable(TDIU16 ${SOURCE_FILES})