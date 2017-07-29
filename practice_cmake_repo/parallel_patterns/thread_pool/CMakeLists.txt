cmake_minimum_required(VERSION 3.5)

project(thread_pool)

file(GLOB THREAD_POOL "*.cpp" "*.hpp" "*.h")

# if(UNIX)
  # set(THREADS_PREFER_PTHREAD_FLAG ON)
  # find_package(Threads REQUIRED)
# endif()

#include_directories(${BOOST_INCLUDE})
#link_directories(${BOOST_LIB})
add_executable(thread_pool ${THREAD_POOL})

# if(UNIX)
  # target_link_libraries(threads_test Threads::Threads)
# endif()

