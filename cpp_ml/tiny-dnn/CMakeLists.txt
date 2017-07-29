cmake_minimum_required(VERSION 3.5)

project(tiny-dnn)

SET(TINY_DNN_INCLUDE ${TINY_DNN_BASE}/include)
#LIST(APPEND CMAKE_MODULE_PATH "${TINY_DNN_BASE}/CMake")
LIST(APPEND CMAKE_PREFIX_PATH "${TINY_DNN_BASE}/CMake")

add_subdirectory(mnist_example)
add_subdirectory(mnist_jni)