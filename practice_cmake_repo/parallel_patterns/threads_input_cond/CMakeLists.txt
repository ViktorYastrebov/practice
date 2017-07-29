cmake_minimum_required(VERSION 3.5)

project(threads_input_cond)

file(GLOB THREADS_INPUT_COND "*.cpp" "*.hpp" "*.h")

if(UNIX)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
endif()

add_executable(threads_input_cond ${THREADS_INPUT_COND})

if(UNIX)
  target_link_libraries(threads_input_cond Threads::Threads)
endif()

