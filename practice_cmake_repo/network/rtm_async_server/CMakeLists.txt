cmake_minimum_required(VERSION 3.5)

project(rtm_async_server)

include_directories(${BOOST_INCLUDE})
link_directories(${BOOST_LIB})

include_directories("../rtm_async_common")
#SET(READ_UTF8_FILE *.cpp *.hpp)
file(GLOB RTM_ASYNC_SERVER "*.cpp" "*.hpp" "*.h")

#add_subdirectory(rtm_async_common)

#add_dependencies(rtm_async_server rtm_async_common)

#include_directories(${ICU_INCLUDE})
#link_directories(${ICU_LIB})

add_executable(rtm_async_server ${RTM_ASYNC_SERVER})

target_link_libraries(rtm_async_server rtm_async_common)
#icu deps
#target_link_libraries(rtm_async_server icuiod.lib)
