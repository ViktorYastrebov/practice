cmake_minimum_required(VERSION 3.5)

project(surface_roughness)

find_package(OpenCV REQUIRED)

#set(MATPLOTLIB_DIR ...)
#$(PYTHON_HOME)\include
#$(PYTHON_HOME)\Lib\site-packages\numpy\core\include

include_directories(${OpenCV_INCLUDE_DIRS} ${MATPLOTLIB_DIR}\include)

#$(PYTHON_HOME)\libs\python36.lib

link_directories(${OpenCV_LIB_DIR} )

file(GLOB SOURCE_FILES "*.cpp" "*.hpp" "*.h")

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${CMAKE_PROJECT_NAME} ${OpenCV_LIBS})