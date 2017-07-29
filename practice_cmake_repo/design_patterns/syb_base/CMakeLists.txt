cmake_minimum_required(VERSION 3.5)

project(syb_base)

#SET(SYB_BASE *.cpp *.hpp)
file(GLOB SYB_BASE "*.cpp" "*.hpp" "*.h")

add_executable(syb_base ${SYB_BASE})