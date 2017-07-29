cmake_minimum_required(VERSION 3.5)
project(qt_table_view) 

set(CMAKE_CXX_STANDARD 14)

#looks like it fix an issue
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Widgets)
file(GLOB QT_TABLE_VIEW "*.cpp" "*.hpp" "*.h" "*.ui")

MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "QT_BIN_DIR: " ${QT_BIN_DIR})
#execute_process(COMMAND ${QT_BIN_DIR}/uic.exe ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui -o ${CMAKE_CURRENT_SOURCE_DIR}/ui_mainwindow.h COMMENT "Building ui_mainwindow.h")

IF(WIN32)
  add_executable(qt_table_view WIN32 ${QT_TABLE_VIEW})
ELSEIF(LINUX)
  add_executable(qt_table_view ${QT_TABLE_VIEW})
ENDIF()
#target_link_libraries(qt_table_view Qt5::Widgets)
