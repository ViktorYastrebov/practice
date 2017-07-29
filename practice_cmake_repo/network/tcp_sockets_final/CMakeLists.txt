cmake_minimum_required(VERSION 3.5)

project(tcp_sockets)

#SET(CMAKE_BUILD_TYPE Release)
#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/bin)
#SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/lib)

#SET(CMAKE_CXX_STANDARD 14)
#SET(CMAKE_CXX_STANDARD_REQUIRED on)

file(GLOB CLIENTS_SOURCES main_client.cpp)
file(GLOB SERVERS_SOURCES main_server.cpp)
file(GLOB COMMON_SOURCES common.cpp common.h socket.cpp socket.h)

add_library(common SHARED ${COMMON_SOURCES})

add_executable(tcp_server ${SERVERS_SOURCES})
add_executable(tcp_client ${CLIENTS_SOURCES})

add_dependencies(tcp_server common)
add_dependencies(tcp_client common)

target_link_libraries(tcp_server common)
target_link_libraries(tcp_client common)


