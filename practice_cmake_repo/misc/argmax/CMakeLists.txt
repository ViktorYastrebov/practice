cmake_minimum_required(VERSION 3.5)

project(argmax)

file(GLOB ARGMAX_SOURCES "*.cpp" "*.hpp" "*.h")
add_executable(argmax ${ARGMAX_SOURCES})