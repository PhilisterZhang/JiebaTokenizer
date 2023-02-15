CMAKE_MINIMUM_REQUIRED (VERSION 3.6)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

project(analysis)

set(VERSION "0.0.1")

add_definitions("-Wall -std=c++11 -O3 -fPIC")

add_subdirectory(src)

add_executable(demo demo.cpp)
# 添加链接库
target_link_libraries(demo nlpanalyzer)
