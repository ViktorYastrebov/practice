cmake_minimum_required(VERSION 3.5)
project(mnist_example)

#set(TinyDNN_USE_SSE ON)
#set(TinyDNN_USE_TBB ON)

#find_package(TinyDNN 1.0.0 REQUIRED)

include_directories(${TINY_DNN_INCLUDE})
#include_directories(${TinyDNN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS})
#link_directories(${TBB_LIB_DIR})

# if(TinyDNN_FOUND)
    file(GLOB MNIST_EXAMPLE_SOURCES "*.cpp" "*.h" "*.hpp")
    add_executable(mnist_example ${MNIST_EXAMPLE_SOURCES})
   # if (${BUILD_TYPE} STREQUAL "debug")
  ##do not use it. The path is wrong. It uses /lib/libtbb.lib for x64. But it must be lib64/libtbb.lib
    # MESSAGE(STATUS "TBB Debug libraries, TBB_DEBUG_LIBRARIES=${TBB_DEBUG_LIBRARIES}")
    # target_link_libraries(mnist_example
                          # tbb_debug.lib
                          # tbbmalloc_debug.lib
    ##                     tbbmalloc_proxy_debug.lib
    ##                     tbbproxy_debug.lib
                          # ) 
  # elseif(${BUILD_TYPE} STREQUAL "release")
    # MESSAGE(STATUS "TBB Release libraries, TBB_LIBRARIES=${TBB_LIBRARIES}")
    # target_link_libraries(mnist_example
						# tbb.lib
						# tbbmalloc.lib
	##				tbbmalloc_proxy.lib
	##				tbbproxy.lib
						# )
  # else()
     # MESSAGE(FATAL_ERROR "BUILD type does not compatible with TBB existent libs")
  # endif()
# endif()
