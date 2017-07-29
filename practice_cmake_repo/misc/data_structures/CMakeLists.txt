cmake_minimum_required(VERSION 3.5)

project(data_structures)

file(GLOB DATA_STRUCTURES_SOURCE "*.cc" "*.hpp" "*.h")
add_executable(data_structures ${DATA_STRUCTURES_SOURCE})
