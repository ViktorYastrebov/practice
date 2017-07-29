cmake_minimum_required(VERSION 3.5)

project(parallel_reduce_tbb)

#find_package(TBB)
include_directories(${TBB_INCLUDE})
link_directories(${TBB_LIBS})

file(GLOB PARALLEL_REDUCE_TBB "*.cpp" "*.hpp" "*.h")
add_executable(parallel_reduce_tbb ${PARALLEL_REDUCE_TBB})

if (${BUILD_TYPE} STREQUAL "debug")
  target_link_libraries(parallel_reduce_tbb tbb.lib)
else()
  target_link_libraries(parallel_reduce_tbb tbb_debug.lib)
endif()
 
