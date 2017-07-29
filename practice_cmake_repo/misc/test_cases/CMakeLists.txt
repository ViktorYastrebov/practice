cmake_minimum_required(VERSION 3.5)

project(test_cases)

file(GLOB TEST_CASES "*.cpp" "*.hpp" "*.h")
add_executable(test_cases ${TEST_CASES})

