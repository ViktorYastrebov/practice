cmake_minimum_required(VERSION 3.5)

project(boost_asio_servers)

file(GLOB BOOST_ASIO_SERVERS "*.cpp" "*.hpp" "*.h")

set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK OFF)

set(Boost_INCLUDE_DIR ${BOOST_INCLUDE})
set(Boost_LIBRARY_DIR ${BOOST_LIB})
find_package(Boost COMPONENTS system filesystem thread REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

if(UNIX)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
endif()



#include_directories(${BOOST_INCLUDE})
#link_directories(${BOOST_LIB})

add_executable(boost_asio_servers ${BOOST_ASIO_SERVERS})
target_link_libraries(boost_asio_servers ${Boost_LIBRARIES})

if(UNIX)
  target_link_libraries(boost_asio_servers Threads::Threads)
endif()

