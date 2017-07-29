cmake_minimum_required(VERSION 3.5)

project(compile_time_map)

include_directories(${BOOST_INCLUDE})
link_directories(${BOOST_LIB})

#SET(TYPE_TRAILS *.cpp *.hpp *.h)
file(GLOB COMPILE_TIME_MAP "*.cpp" "*.hpp" "*.h")

add_executable(compile_time_map ${COMPILE_TIME_MAP})