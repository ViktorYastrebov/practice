cmake_minimum_required(VERSION 3.5)

project(interate_directory_entries)

#include_directories(${BOOST_INCLUDE})
#link_directories(${BOOST_LIB})

#SET(INTERATE_DIRECTORY_ENTRIES *.cpp *.hpp)
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++fs")
file(GLOB INTERATE_DIRECTORY_ENTRIES "*.cpp" "*.hpp" "*.h")

add_executable(interate_directory_entries ${INTERATE_DIRECTORY_ENTRIES})

if(UNIX)
  target_link_libraries(interate_directory_entries stdc++fs)
endif()
