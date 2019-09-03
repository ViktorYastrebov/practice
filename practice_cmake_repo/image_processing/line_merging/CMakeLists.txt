cmake_minimum_required(VERSION 3.5)

project(line_merging)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

file(GLOB SOURCE_FILES "*.cpp" "*.hpp" "*.h")

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${CMAKE_PROJECT_NAME} ${OpenCV_LIBS})