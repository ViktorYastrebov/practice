cmake_minimum_required(VERSION 3.5)

project(rtm_async_client)

include_directories(${BOOST_INCLUDE})
link_directories(${BOOST_LIB})

include_directories("../rtm_async_common")
file(GLOB RTM_ASYNC_CLIENT "*.cpp" "*.hpp" "*.h")

#include_directories(${ICU_INCLUDE})
#link_directories(${ICU_LIB})

add_executable(rtm_async_client ${RTM_ASYNC_CLIENT})
target_link_libraries(rtm_async_client rtm_async_common)

#icu deps
#target_link_libraries(rtm_async_server icuiod.lib)