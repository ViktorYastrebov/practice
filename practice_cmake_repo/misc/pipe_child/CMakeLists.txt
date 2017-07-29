cmake_minimum_required(VERSION 3.5)

project(pipe_child)

file(GLOB READ_UTF8_FILE "*.cpp" "*.hpp" "*.h")
add_executable(pipe_child_example ${READ_UTF8_FILE})

