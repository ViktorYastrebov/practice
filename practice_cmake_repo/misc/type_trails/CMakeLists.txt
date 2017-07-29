cmake_minimum_required(VERSION 3.5)

project(type_trails)

include_directories(${BOOST_INCLUDE})
link_directories(${BOOST_LIB})

#SET(TYPE_TRAILS *.cpp *.hpp *.h)
file(GLOB TYPE_TRAILS "*.cpp" "*.hpp" "*.h")

add_executable(type_trails ${TYPE_TRAILS})