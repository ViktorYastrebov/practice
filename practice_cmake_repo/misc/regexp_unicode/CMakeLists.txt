cmake_minimum_required(VERSION 3.5)

project(regexp_unicode)

include_directories(${ICU_INCLUDE})
link_directories(${ICU_LIB})

#SET(TYPE_TRAILS *.cpp *.hpp *.h)
find_package(Boost COMPONENTS regex)

message(STATUS "Boost_INCLUDE_DIR =${Boost_INCLUDE_DIR}")
#message(STATUS "Boost_LIBRARIES= ${Boost_LIBRARIES}")
include_directories(${Boost_INCLUDE_DIR})
#find_package(ICU 57.1.0 COMPONENTS io uc REQUIRED)

file(GLOB REGEXP_UNICODE "*.cpp" "*.hpp" "*.h")

add_executable(regexp_unicode ${REGEXP_UNICODE})
target_link_libraries(regexp_unicode ${Boost_LIBRARIES} icuiod.lib icuucd.lib icuind.lib)
#target_link_libraries(regexp_unicode ${Boost_LIBRARIES} ${ICU_LIBRARIES})