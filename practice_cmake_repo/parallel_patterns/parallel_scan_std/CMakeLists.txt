cmake_minimum_required(VERSION 3.5)

project(parallel_scan_std)

file(GLOB PARALLLEL_REDUCE "*.cpp" "*.hpp" "*.h")
add_executable(parallel_scan_std ${PARALLLEL_REDUCE})
