cmake_minimum_required(VERSION 3.5)

project(multiprecision_mpir)

file(GLOB MULTIPRECISION_MPIR "*.cpp" "*.hpp" "*.h")

include_directories(${MPIR_INCLUDE})
link_directories(${MPIR_LIB})

add_executable(multiprecision_mpir ${MULTIPRECISION_MPIR})

if(WIN32)
  target_link_libraries(multiprecision_mpir mpir.lib mpirxx.lib)
endif()

