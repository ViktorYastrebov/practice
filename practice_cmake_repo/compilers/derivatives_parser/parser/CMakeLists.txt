cmake_minimum_required(VERSION 3.5)

project(derives_parser)

file(GLOB PARSER_SOURCES "*.cpp" ".cc")
file(GLOB PARSER_HEADERS "*.h" "*.hpp")

file(GLOB LEXER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../lexer/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/../lexer/*.cc" "${CMAKE_CURRENT_SOURCE_DIR}/../lexer/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/lexer/*.hpp")

include_directories(${LOKI_INCLUDE} ${QUEX_DIR} ../parser_objects ../lexer)
link_directories(${LOKI_LIB})

add_library(derives_parser STATIC ${PARSER_SOURCES} ${PARSER_HEADERS} ${LEXER_SOURCES})

add_custom_command(
  TARGET derives_parser PRE_BUILD
  COMMAND bison24 ARGS -d -b DerivParser -o DerivParser.cpp original_gramma.yy
  DEPENDS original_gramma.yy
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command(
 TARGET derives_parser PRE_BUILD
 ##test#OUTPUT DerivLexer DerivLexer-configuration DerivLexer-token DerivLexer-token_ids DerivLexer.cpp
 COMMAND python-2.7 ARGS ${QUEX_DIR}/quex-exe.py --codec utf16 -b 2 -i lexer.quex --token-id-prefix "KW_" --foreign-token-id-file tokens.hpp -o DerivLexer
 DEPENDS lexer.quex tokens.hpp
 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lexer
)

if(WIN32)
   target_link_libraries(derives_parser derivatives_parser_objects)
elseif(UNIX)
 #
endif()
