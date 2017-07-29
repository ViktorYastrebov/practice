cmake_minimum_required(VERSION 3.5)

project(std_wait_for)

file(GLOB STD_WAIT_FOR "*.cpp" "*.hpp" "*.h")

add_executable(std_wait_for ${STD_WAIT_FOR})
