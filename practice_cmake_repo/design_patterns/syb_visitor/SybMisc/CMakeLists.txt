cmake_minimum_required(VERSION 3.5)

project(libsybmisc)

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR} ${LOKI_INCLUDE})
link_directories(${Boost_LIBRARY_DIR})

file(GLOB MISC_SOURCES "*.c" "*.cpp" "*.hpp" "*.h")
add_library(libsybmisc STATIC ${MISC_SOURCES})
