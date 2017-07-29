cmake_minimum_required(VERSION 3.5)

project(derivatives_parser_objects)

file(GLOB PARSER_OBJECTS_SOURCE "*.cpp" ".cc")
file(GLOB PARSER_OBJECTS_HEADERS "*.h" "*.hpp")

MESSAGE(STATUS ${PARSER_OBJECTS_HEADERS})

include_directories(${LOKI_INCLUDE})
link_directories(${LOKI_LIB})

add_library(derivatives_parser_objects STATIC ${PARSER_OBJECTS_SOURCE} ${PARSER_OBJECTS_HEADERS})

#Loki deps
if(WIN32)
  target_link_libraries(derivatives_parser_objects ${LOKI_LINK_LIB})
elseif(UNIX)
  target_link_libraries(derivatives_parser_objects loki)
endif()

