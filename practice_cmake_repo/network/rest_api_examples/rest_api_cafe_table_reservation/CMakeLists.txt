cmake_minimum_required(VERSION 3.5)

project(rest_api_cafe_table_reservation)

include_directories(${REST_BED_INCLUDE})
link_directories(${REST_BED_LIB})

file(GLOB EXAMPLE_SOURCES "*.cpp" "*.hpp")

add_executable(rest_api_cafe_table_reservation ${EXAMPLE_SOURCES})
target_link_libraries(rest_api_cafe_table_reservation restbed)


