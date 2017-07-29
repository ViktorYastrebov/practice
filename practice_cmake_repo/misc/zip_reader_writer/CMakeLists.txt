cmake_minimum_required(VERSION 3.5)

project(zip_reader_writer)

#boost
include_directories(${BOOST_INCLUDE})
link_directories(${BOOST_LIB})

#libzip
include_directories(${LIB_ZIP_INCLUDE} ${ZLIB_INCLUDE})
link_directories(${ZIP_LIB} ${LIB_ZIP_LIB})

#SET(ZIP_READER_WRITER *.cpp *.h)
file(GLOB ZIP_READER_WRITER "*.cpp" "*.hpp" "*.h")

add_executable(zip_reader_writer ${ZIP_READER_WRITER})

#LIB_ZIP deps
if(WIN32)
  target_link_libraries(zip_reader_writer zip.lib)
elseif(UNIX)
  target_link_libraries(zip_reader_writer stdc++fs libz.so libzip.so)
endif()
