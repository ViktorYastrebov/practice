cmake_minimum_required(VERSION 3.5)

project(libsybboostserialization)

include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${ICU_INCLUDE})

find_package(Boost COMPONENTS system filesystem serialization REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

file(GLOB SYB_SOURCES "*.cpp")
add_library(libsybboostserialization STATIC ${SYB_SOURCES})
