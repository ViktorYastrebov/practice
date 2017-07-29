cmake_minimum_required(VERSION 3.5)

project(odbc_mssql)


#find_package(Boost COMPONENT multiprecision)
# format library is header only
include_directories(${Boost_INCLUDE_DIR})

file(GLOB ODBC_MSSQL_EXAMPLE "*.cpp" "*.hpp" "*.h")
add_executable(odbc_mssql ${ODBC_MSSQL_EXAMPLE})

if(WIN32)
 target_link_libraries(odbc_mssql odbc32.lib)
elseif(UNIX)
#has not been tested yet
 target_link_libraries(odbc_mssql odbc.so)
endif()

