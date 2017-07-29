cmake_minimum_required(VERSION 3.5)

project(curring)

file(GLOB CURRING_SOURCE "*.cpp" "*.hpp" "*.h")
add_executable(curring ${CURRING_SOURCE})