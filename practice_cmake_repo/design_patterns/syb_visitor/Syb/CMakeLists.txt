cmake_minimum_required(VERSION 3.5)
project(libsyb)


#Libs: boost/ICU common/loki/rapidxml
# remove include_directories()
include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${ICU_INCLUDE} ${LOKI_INCLUDE} ${RAPID_XML})

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

#sources
file(GLOB SYB_BASE "*.cpp" "*.cc" "*.h" "*.c") #REMOVE Binary.h,InitFromBinary.h

add_library(libsyb STATIC ${SYB_BASE})

add_dependencies(libsyb libsybmisc)
