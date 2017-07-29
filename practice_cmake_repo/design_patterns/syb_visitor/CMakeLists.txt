cmake_minimum_required(VERSION 3.5)

project(Syb_Proj)

if(WIN32)
  add_definitions(-DAXIS2_SKIP_INT_TYPEDEFS)
endif(WIN32)

add_subdirectory(SybMisc)
add_subdirectory(Syb)
add_subdirectory(SybXml2)
add_subdirectory(SybJni)
add_subdirectory(SybBoostSerialization)
add_subdirectory(SybTests/SybXml2Test)