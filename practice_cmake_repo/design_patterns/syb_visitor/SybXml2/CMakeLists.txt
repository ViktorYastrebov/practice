#generated by GenQxBuildFiles
cmake_minimum_required(VERSION 3.5)

project(libsybxml)

include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${ICU_INCLUDE} ${LOKI_INCLUDE} ${RAPID_XML})

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

file(GLOB SYB_XML "*.c" "*.cpp" "*.inl")

add_library(libsybxml STATIC ${SYB_XML})
add_dependencies(libsybxml libsyb)
  
