cmake_minimum_required(VERSION 3.5)

project(restbed_example1)

include_directories(${REST_BED_INCLUDE})
link_directories(${REST_BED_LIB})

file(GLOB EXAMPLE_SOURCES "*.cpp" "*.hpp")

add_executable(restbed_example1 ${EXAMPLE_SOURCES})
target_link_libraries(restbed_example1 restbed)


