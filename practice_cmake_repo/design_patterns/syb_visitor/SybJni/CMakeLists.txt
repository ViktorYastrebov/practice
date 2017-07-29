cmake_minimum_required(VERSION 3.5)

project(libsybjni)

include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${ICU_INCLUDE} ${LOKI_INCLUDE})

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

find_package(JNI)
if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
else()
    MESSAGE(FATAL_ERROR "Can't find JNI Package")
endif()

include_directories(${JNI_INCLUDE_DIRS})
link_directories(${JNI_LIBRARIES})

# boost/loki/icu common/jni
file(GLOB SYB_JNI "*.cpp")

add_library(libsybjni STATIC ${SYB_JNI}
   #DEPENDS libsyb
   )
#add_dependencies(libsybjni libmisc)
