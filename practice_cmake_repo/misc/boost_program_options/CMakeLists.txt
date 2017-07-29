cmake_minimum_required(VERSION 3.5)

project(boost_program_options)

include_directories(${BOOST_INCLUDE})
link_directories(${BOOST_LIB})

find_package(Boost COMPONENTS program_options REQUIRED)

#SET(TYPE_TRAILS *.cpp *.hpp *.h)
file(GLOB PO_EXAMPLE "*.cpp" "*.hpp" "*.h")

add_executable(boost_program_options ${PO_EXAMPLE})
target_link_libraries(boost_program_options ${Boost_LIBRARIES})
