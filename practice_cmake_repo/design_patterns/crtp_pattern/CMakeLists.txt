cmake_minimum_required(VERSION 3.5)
project(crtp_pattern)
file(GLOB CRTP_PATTERN "*.cpp" "*.hpp" "*.h")
add_executable(crtp_pattern ${CRTP_PATTERN})