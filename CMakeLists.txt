cmake_minimum_required(VERSION 3.4)
project(LinuxThreads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES main.c)
add_executable(LinuxThreads ${SOURCE_FILES})