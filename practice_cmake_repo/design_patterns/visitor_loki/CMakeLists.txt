cmake_minimum_required(VERSION 3.5)

project(visitor_loki)

include_directories(${LOKI_INCLUDE})
link_directories(${LOKI_LIB})

#SET(VISITOR_LOKI *.cpp *.hpp *.h)
file(GLOB VISITOR_LOKI "*.cpp" "*.hpp" "*.h")

add_executable(visitor_loki ${VISITOR_LOKI})

#Loki deps
if(WIN32)
  target_link_libraries(visitor_loki loki.lib)
elseif(UNIX)
  target_link_libraries(visitor_loki loki)
endif()

