cmake_minimum_required(VERSION 3.5)

project(dlib_face_detection)

add_definitions(-DDLIB_JPEG_SUPPORT)

include_directories(${DLIB_INCLUDE})
link_directories(${DLIB_LIB})

#SET(INTERATE_DIRECTORY_ENTRIES *.cpp *.hpp)
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++fs")
file(GLOB DLIB_FACE_DETECTION "*.cpp" "*.hpp" "*.h")

add_executable(dlib_face_detection ${DLIB_FACE_DETECTION})

IF(WIN32)
  target_link_libraries(dlib_face_detection dlib.lib)
ENDIF()
