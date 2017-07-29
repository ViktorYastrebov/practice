cmake_minimum_required(VERSION 3.5)

project(odbc_test)

file(GLOB ODBC_TESTS "*.cpp" "*.hpp" "*.h")
add_executable(odbc_tests ${ODBC_TESTS})

if(WIN32)
 target_link_libraries(odbc_tests odbc32.lib)
elseif(UNIX)
#has not been tested yet
 target_link_libraries(odbc_tests odbc.so)
endif()

