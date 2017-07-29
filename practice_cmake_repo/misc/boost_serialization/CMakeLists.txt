cmake_minimum_required(VERSION 3.5)

project(boost_serialization)

#include_directories(${BOOST_INCLUDE})
#link_directories(${BOOST_LIB})

set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK OFF)

set(Boost_INCLUDE_DIR ${BOOST_INCLUDE})
set(BOOST_LIBRARYDIR ${BOOST_LIB})
#set(Boost_LIBRARY_DIR ${BOOST_LIB})
find_package(Boost COMPONENTS serialization REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

file(GLOB BOOST_SERIALIZATION "*.cpp" "*.hpp" "*.h")

add_executable(boost_serialization ${BOOST_SERIALIZATION})
target_link_libraries(boost_serialization ${Boost_LIBRARIES})

