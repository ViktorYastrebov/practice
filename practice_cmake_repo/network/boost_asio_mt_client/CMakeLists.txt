cmake_minimum_required(VERSION 3.5)

project(boost_asio_mt_client)

#set(Boost_NO_SYSTEM_PATHS ON)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_STATIC_RUNTIME OFF)
#set(BOOST_ALL_DYN_LINK OFF)

find_package(Boost COMPONENTS 
	system
	#filesystem
	#thread
	#serialization
	REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

if(UNIX)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
endif()

file(GLOB CLIENT_SOURCES "*.cpp" "*.hpp" "*.h" "*.c")

add_executable(boost_asio_mt_client ${CLIENT_SOURCES})

