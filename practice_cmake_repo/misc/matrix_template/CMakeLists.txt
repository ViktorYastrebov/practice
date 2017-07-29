cmake_minimum_required(VERSION 3.5)

project(matrix_template)

find_package(Boost COMPONENTS unit_test_framework)

message(STATUS "Boost_INCLUDE_DIR =${Boost_INCLUDE_DIR}")
message(STATUS "Boost_LIBRARIES= ${Boost_LIBRARIES}")

include_directories(${Boost_INCLUDE_DIR})

file(GLOB MATRIX_TEMPLATE_SOURCES "*.cc" "*.hpp" "*.h")
add_executable(matrix_template ${MATRIX_TEMPLATE_SOURCES})

target_link_libraries(matrix_template ${Boost_LIBRARIES})