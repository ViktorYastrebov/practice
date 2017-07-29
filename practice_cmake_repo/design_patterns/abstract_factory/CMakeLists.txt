cmake_minimum_required(VERSION 3.5)

project(abstract_factory)

#include_directories(${BOOST_INCLUDE})
#link_directories(${BOOST_LIB})

#SET(ABSTRACT_FACTORY *.cpp *.hpp *.h)
file(GLOB ABSTRACT_FACTORY "*.cpp" "*.hpp" "*.h")

add_executable(abstract_factory ${ABSTRACT_FACTORY})