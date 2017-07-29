cmake_minimum_required(VERSION 3.5)
project(derivatives_parser)

add_subdirectory(parser_objects)
add_subdirectory(parser)
add_subdirectory(codegen)



include_directories(${QUEX_DIR} ./lexer ${LOKI_INCLUDE})
link_directories(${LOKI_LIB})

file(GLOB DERIVATIVES_PARSER "*.cpp" "*.hpp" "*.h")

add_executable(derivatives_parser ${DERIVATIVES_PARSER})
target_link_libraries(derivatives_parser derives_parser derivatives_parser_objects)
