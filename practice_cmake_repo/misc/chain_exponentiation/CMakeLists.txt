cmake_minimum_required(VERSION 3.5)

project(chain_exponentiation)

#include_directories(${BOOST_INCLUDE})
#link_directories(${BOOST_LIB})

file(GLOB CHAIN_EXPONENTIATION "*.cpp" "*.hpp" "*.h")

add_executable(chain_exponentiation ${CHAIN_EXPONENTIATION})