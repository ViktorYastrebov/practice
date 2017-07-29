cmake_minimum_required(VERSION 3.5)

project(threads_test)

#include_directories(${BOOST_INCLUDE})
#link_directories(${BOOST_LIB})

#SET(THREADS_TEST *.cpp *.hpp *.h)
file(GLOB THREADS_TEST "*.cpp" "*.hpp" "*.h")

if(UNIX)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
endif()

#include_directories(${BOOST_INCLUDE})
#link_directories(${BOOST_LIB})
add_executable(threads_test ${THREADS_TEST})

if(UNIX)
  target_link_libraries(threads_test Threads::Threads)
endif()

