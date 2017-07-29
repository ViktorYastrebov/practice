cmake_minimum_required(VERSION 3.5)

project(map_conditon)

#include_directories(${BOOST_INCLUDE})
#link_directories(${BOOST_LIB})

#SET(MAP_CONDITON *.cpp *.hpp)
file(GLOB MAP_CONDITON "*.cpp" "*.hpp" "*.h")
add_executable(map_conditon ${MAP_CONDITON})