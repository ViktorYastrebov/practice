cmake_minimum_required(VERSION 3.5)

project(canny_detector)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

file(GLOB CANNY_DETECTOR "*.cpp" "*.hpp" "*.h")
add_executable(canny_detector ${CANNY_DETECTOR})

target_link_libraries(canny_detector ${OpenCV_LIBS})