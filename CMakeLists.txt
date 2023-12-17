cmake_minimum_required(VERSION 3.14)
project(binner)

add_executable(binner binner.cpp blob.o)
add_executable(binView binView.cpp)

