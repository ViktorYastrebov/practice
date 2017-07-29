cmake_minimum_required(VERSION 3.5)

project(write_utf8_file)

#include_directories(${BOOST_INCLUDE})
#link_directories(${BOOST_LIB})

#SET(WRITE_UTF8_FILE *.cpp *.hpp)
file(GLOB WRITE_UTF8_FILE "*.cpp" "*.hpp" "*.h")

include_directories(${ICU_INCLUDE})
link_directories(${ICU_LIB})

add_executable(write_utf8_file ${WRITE_UTF8_FILE})

#ICU deps

#icu deps
#target_link_libraries(read_utf8_file ICU::ICU)
if(WIN32)
  target_link_libraries(write_utf8_file icuiod.lib)
elseif(UNIX)
  target_link_libraries(write_utf8_file icuio.so)
endif()

