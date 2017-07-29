cmake_minimum_required(VERSION 3.5)

project(oracle_oci)

if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL")
  add_definitions(-DUNICODE -D_UNICODE)
elseif(UNIX)
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED on)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")
endif()

if(DEFINED $ENV{ORACLE_HOME})
  SET(ORACLE_ROOT $ENV{ORACLE_HOME})
else()
  message(STATUS "There is used default path")
  SET(ORACLE_ROOT d:/rdbms/oracle_express_11g/app/oracle/product/11.2.0/server)
endif()

SET(ORACLE_INCLUDE ${ORACLE_ROOT}/oci/include)
SET(ORACLE_LIB_PATH ${ORACLE_ROOT}/oci/lib/MSVC/)

include_directories(${ORACLE_INCLUDE})
link_directories(${ORACLE_LIB_PATH})

file(GLOB ORACLE_OCI_SOURCES "*.cpp" "*.hpp" "*.h")
add_executable(oracle_oci ${ORACLE_OCI_SOURCES})

if(WIN32)
  target_link_libraries(oracle_oci oci.lib)
elseif(UNIX)
   message(FATAL_ERROR "currently it's not tested under any Linux Distro")
  #target_link_libraries(oracle_oci oci)
endif()

