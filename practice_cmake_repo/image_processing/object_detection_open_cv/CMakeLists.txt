cmake_minimum_required(VERSION 3.5)

project(object_detection_open_cv)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

file(GLOB OBJECT_DETECTION_OPEN_CV "*.cpp" "*.hpp" "*.h")
add_executable(object_detection_open_cv ${OBJECT_DETECTION_OPEN_CV})

target_link_libraries(object_detection_open_cv ${OpenCV_LIBS})