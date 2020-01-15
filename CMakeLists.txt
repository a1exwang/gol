cmake_minimum_required(VERSION 3.0)
project(gol)
set(CMAKE_CXX_STANDARD 20)

add_executable(gol main.cpp instruction.cpp thread.cpp common.cpp)
target_link_libraries(gol PRIVATE glog)
