cmake_minimum_required(VERSION 3.16)
project(copy)

add_executable(FileCopy copy.c)
target_link_libraries(FileCopy -lreadline)
set(CMAKE_BUILD_TYPE Debug)