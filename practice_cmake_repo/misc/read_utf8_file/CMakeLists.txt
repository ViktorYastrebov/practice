cmake_minimum_required(VERSION 3.5)

project(read_utf8_file)

#include_directories(${BOOST_INCLUDE})
#link_directories(${BOOST_LIB})

#SET(READ_UTF8_FILE *.cpp *.hpp)
file(GLOB READ_UTF8_FILE "*.cpp" "*.hpp" "*.h")

#SET(ICU_ROOT_DIR ${ICU_DIR})
#SET(ICU_INCLUDE_DIR  ${ICU_INCLUDE})
#SET(ICU_LIBRARY ${ICU_LIB})


#find_package(ICU 57.1.0 COMPONENTS io REQUIRED)

include_directories(${ICU_INCLUDE})
link_directories(${ICU_LIB})

add_executable(read_utf8_file ${READ_UTF8_FILE})

#icu deps
#target_link_libraries(read_utf8_file ICU::ICU)
if(WIN32)
  target_link_libraries(read_utf8_file icuiod.lib)
elseif(UNIX)
  target_link_libraries(read_utf8_file icuio.so)
endif()
