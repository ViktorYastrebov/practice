cmake_minimum_required(VERSION 3.5)
project(libsybxml2test)

include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${ICU_INCLUDE} ${LOKI_INCLUDE})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

link_directories(${ICU_LIB})

file(GLOB SYBXML_TEST "*.cpp")
file(GLOB_RECURSE XML_FILES "*.xml")
add_executable(libsybxml2test ${SYBXML_TEST} ${XML_FILES})

target_link_libraries(libsybxml2test libsybxml libsyb libsybmisc icuiod icuucd)


#comp flags
 # QxUsePackage(boost)
 # QxUsePackage(boost PART test_exec_monitor)
 # QxUsePackage(icu PART common)
 # QxUsePackage(axis2)
 # QxUsePackage(loki)
# include_directories(.
 # ${Base_Proj_SOURCE_DIR}
 # ${SybX_Proj_SOURCE_DIR}
 # ${Hydra5_Proj_BINARY_DIR}
# )
#sources
# AddSrcByMask(sybxmltestSrc
  # *.cpp
# )

#target
# AddExecutable(sybxml2test ${sybxmltestSrc} DEPENDS  libmisc libfglmisc libsybxml)
# set_target_properties(sybxml2test PROPERTIES EXCLUDE_FROM_ALL 1)

# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/IndependentOrderTest DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#dependencies

###############end target:"sybxml2test"#########################
