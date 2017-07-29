cmake_minimum_required(VERSION 3.5)

project(practice_cmake)

#searching for FindICU,cmake
#set(CMAKE_MODULE_PATH ${} ${PROJECT_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})


#IDEA: for the particular example can make the srcipt which called the project by it's folder name
SET(CMAKE_BUILD_TYPE Debug)
#SET(CMAKE_BUILD_TYPE Release)
#SET(SDKS_DIR "d:/sdks")
string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

IF(WIN32)
  MESSAGE(STATUS "WIN32 passes")
  SET(SDKS_DIR "d:/sdks")
  SET(BUILD_TOOL "vc14")
ELSEIF(UNIX)
  SET(SDKS_DIR "/home/victor/sdks")
  SET(BUILD_TOOL "gcc")
ENDIF()

MESSAGE(STATUS "SDKS_DIR :" ${SDKS_DIR})

IF(WIN32)
  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
#    SET(BIT_CAPACITY 64)
    SET(LIB_POSTFIX "64")
  ELSE()
#    SET(BIT_CAPACITY 32)
    SET(LIB_POSTFIX "")
  ENDIF()
ELSE(UNIX)
  SET(LIB_POSTFIX "")
ENDIF()

if(MSVC)
  #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB") 
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL")
  #SET(CMAKE_TARGET_DEFINITIONS "/D_UNICODE") 
  add_definitions(-DUNICODE -D_UNICODE)
elseif(UNIX)

  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED on)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")
#   include(CheckCXXCompilerFlag)
#   CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
#   if(COMPILER_SUPPORTS_CXX11)
#    message(STATUS "applying -std=c++11")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#   endif()
endif()


SET(BOOST_ROOT ${SDKS_DIR}/${BUILD_TOOL}/boost-1.61/${BUILD_TYPE})

#base libraries
# BOOST
SET(BOOST_DIR ${SDKS_DIR}/${BUILD_TOOL}/boost-1.61/${BUILD_TYPE})
SET(BOOST_INCLUDE ${BOOST_DIR}/include)
SET(BOOST_LIB ${BOOST_DIR}/lib${LIB_POSTFIX})

# GENERAL
set(BOOST_ROOT ${BOOST_DIR})

set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK OFF)

set(Boost_INCLUDE_DIR ${BOOST_INCLUDE})
set(Boost_LIBRARY_DIR ${BOOST_LIB})


# ICU 57.1
SET(ICU_DIR ${SDKS_DIR}/${BUILD_TOOL}/icu-57.1/${BUILD_TYPE})
SET(ICU_INCLUDE ${ICU_DIR}/include)
SET(ICU_LIB ${ICU_DIR}/lib${LIB_POSTFIX})

SET(ICU_ROOT ${ICU_DIR})
SET(ICU_LIBRARIES ${ICU_LIB})

MESSAGE(STATUS "BUILD_TYPE=${BUILD_TYPE}")
IF(WIN32)
  SET(LOKI_DIR ${SDKS_DIR}/${BUILD_TOOL}/loki-0.1.7/${BUILD_TYPE})
  SET(LOKI_INCLUDE ${LOKI_DIR}/include)
  SET(LOKI_LIB ${LOKI_DIR}/lib${LIB_POSTFIX})
  if (${BUILD_TYPE} STREQUAL "debug")
    SET(LOKI_LINK_LIB loki_D.lib)
    MESSAGE(STATUS "LOKI_LINK_LIB=${LOKI_LINK_LIB}")
  else()
    SET(LOKI_LINK_LIB loki.lib)
  endif()
  #SET(LOKI_DIR ${SDKS_DIR}/loki--0.1.7.built)
  #SET(LOKI_INCLUDE ${LOKI_DIR}/include)
  #SET(LOKI_LIB ${LOKI_DIR}/lib)
ELSEIF(UNIX)
  SET(LOKI_DIR ${SDKS_DIR}/${BUILD_TOOL}/loki-0.1.7)
  SET(LOKI_INCLUDE ${LOKI_DIR}/include)
  SET(LOKI_LIB ${LOKI_DIR}/lib)
ENDIF()

if(UNIX)
  SET(ZIP_INCLUDE ${SDKS}/zlib-1.2.8/include)
  SET(ZIP_LIB ${SDKS}/zlib-1.2.8/lib)
endif()

# Libzip --use only release. For debug there is a symbolic link.
SET(LIB_ZIP ${SDKS_DIR}/${BUILD_TOOL}/libzip/${BUILD_TYPE})
SET(LIB_ZIP_INCLUDE ${LIB_ZIP}/include)
SET(LIB_ZIP_LIB ${LIB_ZIP}/lib${LIB_POSTFIX})

#rapidxml
SET(RAPID_XML ${SDKS_DIR}/${BUILD_TOOL}/rapidxml)

#JNI
SET(JAVA_HOME "c:/Progra~1/Java/jdk1.8.0_91")

#rest-sdks
SET(REST_SDK ${SDKS_DIR}/${BUILD_TOOL}/rest-sdks/${BUILD_TYPE})
SET(REST_SDK_INCLUDE ${SDKS_DIR}/${BUILD_TOOL}/rest-sdks/${BUILD_TYPE}/include)
SET(REST_SDK_LIB ${SDKS_DIR}/${BUILD_TOOL}/rest-sdks/${BUILD_TYPE}/lib${LIB_POSTFIX})

#restbed
SET(REST_BED ${SDKS_DIR}/${BUILD_TOOL}/restbed/${BUILD_TYPE})
SET(REST_BED_INCLUDE ${SDKS_DIR}/${BUILD_TOOL}/restbed/${BUILD_TYPE}/include)
SET(REST_BED_LIB ${SDKS_DIR}/${BUILD_TOOL}/restbed/${BUILD_TYPE}/lib${LIB_POSTFIX})

#set output dirs
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/lib)


IF(WIN32)
  SET(QT_DIR ${SDKS_DIR}/${BUILD_TOOL}/qt-5.6.1)
  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(QT_BIN_DIR ${QT_DIR}/5.7/msvc2015_64/bin)
  ELSE()
    SET(QT_BIN_DIR ${QT_DIR}/5.7/msvc2015/bin)
  ENDIF()
  SET(QT_TOOLS ${QT_DIR}/Tools/QtCreator/bin)
  SET(OPEN_VC_DIR ${SDKS_DIR}/opencv-3.1.0/build)
ELSEIF(UNIX)
  SET(QT_DIR ${SDKS_DIR}/${BUILD_TOOL}/qt-5.7)
  MESSAGE(STATUS "QT_DIR :" ${QT_DIR})
  #/home/victor/sdks/gcc/qt-5.7/5.7/gcc_64/bin
  SET(QT_BIN_DIR ${QT_DIR}/5.7/gcc_64/bin)
  SET(QT_TOOLS ${QT_DIR}/Tools/QtCreator/bin)
ENDIF()

#dlib-19.0
IF(WIN32)
  SET(DLIB_BASE ${SDKS_DIR}/${BUILD_TOOL}/dlib-19.0/${BUILD_TYPE})
  SET(DLIB_INCLUDE ${DLIB_BASE}/include)
  SET(DLIB_LIB ${DLIB_BASE}/lib${LIB_POSTFIX})
ENDIF()


#http://doc.qt.io/qt-5/cmake-manual.html
#QT-5.7
IF(WIN32)
  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    MESSAGE(STATUS "use QT x64")
    SET(CMAKE_PREFIX_PATH ${QT_DIR}/5.7/msvc2015_64/lib/cmake)
  ELSE()
    SET(CMAKE_PREFIX_PATH ${QT_DIR}/5.7/msvc2015/lib/cmake)
  ENDIF()
  SET(CMAKE_PREFIX_PATH ${OPEN_VC_DIR})
ELSEIF(UNIX)
  SET(CMAKE_PREFIX_PATH ${QT_DIR}/5.7/gcc_64/lib/cmake)
ENDIF()

#QUEX-0.65.11
if(WIN32)
  SET(QUEX_DIR d:/progs/quex-0.65.11)
  SET(ORACLE_BASE d:/rdbms/oracle_express_11g/app/oracle/product/11.2.0/server/)
  SET(ORACLE_INCLUDE ${ORACLE_BASE}/oci/include)
  SET(ORACLE_LIB ${ORACLE_BASE}/oci/lib/MSVC/)
ELSE(UNIX)
ENDIF()

#TTB 
SET(TBB_BASE ${SDKS_DIR}/${BUILD_TOOL}/tbb)
SET(TBB_INCLUDE ${TBB_BASE}/include)
SET(TBB_LIBS ${TBB_BASE}/lib${LIB_POSTFIX})

#MPIR-2.7.2
SET(MPIR_BASE ${SDKS_DIR}/${BUILD_TOOL}/mpir-2.7.2/${BUILD_TYPE})
SET(MPIR_INCLUDE ${MPIR_BASE}/include)
SET(MPIR_LIB ${MPIR_BASE}/lib${LIB_POSTFIX})

#LLVM-3.9
SET(LLVM_BASE ${SDKS_DIR}/${BUILD_TOOL}/llvm-3.9/${BUILD_TYPE})
SET(LLVM_INCLUDE ${LLVM_BASE}/include)
SET(LLVM_LIBS ${LLVM_BASE}/lib${LIB_POSTFIX})


# add_subdirectory(argmax)
# add_subdirectory(abstract_factory)
# add_subdirectory(binds)
# add_subdirectory(boost_asio_clients)
# add_subdirectory(boost_asio_servers)
# add_subdirectory(boost_serialization)
# add_subdirectory(chain_exponentiation)
# add_subdirectory(interate_directory_entries)
# add_subdirectory(map_conditon)
#add_subdirectory(python_fs_xml)
# add_subdirectory(read_utf8_file)

#async server example
# add_subdirectory(rtm_async_common)
# add_subdirectory(rtm_async_server)
# add_subdirectory(rtm_async_client)
#set deps
# add_dependencies(rtm_async_server rtm_async_common)
# add_dependencies(rtm_async_client rtm_async_common)

# add_subdirectory(syb_base)

#temporary remove from linux build
# IF(WIN32)
  # add_subdirectory(syb_modified)
  # add_subdirectory(syb_visitor)
  # add_subdirectory(oracle_examples)
  # add_subdirectory(rest_api_examples)
# ENDIF() 

# add_subdirectory(threads_test)
# add_subdirectory(type_trails)
# add_subdirectory(visitor_loki)
# add_subdirectory(write_utf8_file)
# add_subdirectory(zip_reader_writer)

# add_subdirectory(qt_table_view)
# add_subdirectory(object_detection_open_cv)
# add_subdirectory(dlib_face_detection)

# add_subdirectory(derivatives_parser)
# add_subdirectory(crtp_pattern)
# add_subdirectory(parallel_patterns)


add_subdirectory(compilers)
add_subdirectory(design_patterns)
add_subdirectory(dlib_face_detection)
add_subdirectory(gui)
add_subdirectory(misc)
add_subdirectory(network)
add_subdirectory(image_processing)
add_subdirectory(parallel_patterns)
if(WIN32)
  add_subdirectory(sql)
endif()

