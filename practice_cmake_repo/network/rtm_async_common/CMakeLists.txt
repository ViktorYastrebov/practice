cmake_minimum_required(VERSION 3.5)

project(rtm_async_common)

set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK OFF)

set(Boost_INCLUDE_DIR ${BOOST_INCLUDE})
set(Boost_LIBRARY_DIR ${BOOST_LIB})
find_package(Boost COMPONENTS system filesystem thread serialization REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

if(UNIX)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
endif()

#include_directories(${BOOST_INCLUDE})
#link_directories(${BOOST_LIB})

#SET(READ_UTF8_FILE *.cpp *.hpp)
file(GLOB RTM_ASYNC_COMMON_SOURCES "*.cpp")
file(GLOB RTM_ASYNC_COMMON_HEADERS "*.h" "*.hpp")

#include_directories(${ICU_INCLUDE})
#link_directories(${ICU_LIB})

add_library(rtm_async_common STATIC ${RTM_ASYNC_COMMON_SOURCES} ${RTM_ASYNC_COMMON_HEADERS})

#target_include_directories(rtm_async_common PUBLIC ${RTM_ASYNC_COMMON_HEADERS})
#icu deps
#target_link_libraries(rtm_async_server icuiod.lib)

target_link_libraries(rtm_async_common ${Boost_LIBRARIES})

IF(WIN32)
  target_link_libraries(rtm_async_common Netapi32.lib)
ELSEIF(UNIX)
  target_link_libraries(rtm_async_common Threads::Threads)
ENDIF()

