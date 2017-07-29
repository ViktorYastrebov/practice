cmake_minimum_required(VERSION 3.5)
project(libdigit_recog)

add_definitions(-DDIGIT_RECOGNITION_EXPORTS)

include_directories(${TINY_DNN_INCLUDE})

file(GLOB LIBDIGIT_RECOG_SOURCES "*.cpp" "*.h" "*.hpp")
add_library(libdigit_recog SHARED ${LIBDIGIT_RECOG_SOURCES})

