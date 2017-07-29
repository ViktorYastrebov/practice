cmake_minimum_required(VERSION 3.5)

project(syb_modified)

#include_directories(${BOOST_INCLUDE})
#link_directories(${BOOST_LIB})

#SET(SYB_MODIFIED *.cpp *.hpp)
file(GLOB SYB_MODIFIED "*.cpp" "*.hpp" "*.h")

add_executable(syb_modified ${SYB_MODIFIED})