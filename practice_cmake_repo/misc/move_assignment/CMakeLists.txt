cmake_minimum_required(VERSION 3.5)

project(move_assignment)


if(UNIX)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
endif()


file(GLOB MOVE_ASSIGNMENT "*.cpp" "*.hpp" "*.h")
add_executable(move_assignment ${MOVE_ASSIGNMENT})


if(UNIX)
 target_link_libraries(move_assignment Threads::Threads)
endif()

