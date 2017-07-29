cmake_minimum_required(VERSION 3.5)
project(jni_wrapper)

# >javac.exe JNIDigitRecog.java
# >javah -jni JNIDigitRecog 
find_package(JNI REQUIRED)

# JNI_INCLUDE_DIRS      = the include dirs to use
# JNI_LIBRARIES         = the libraries to use
# JNI_FOUND             = TRUE if JNI headers and libraries were found.
# JAVA_AWT_LIBRARY      = the path to the jawt library
# JAVA_JVM_LIBRARY      = the path to the jvm library
# JAVA_INCLUDE_PATH     = the include path to jni.h
# JAVA_INCLUDE_PATH2    = the include path to jni_md.h
# JAVA_AWT_INCLUDE_PATH = the include path to jawt.h
if(JNI_FOUND)
  include_directories(${JNI_INCLUDE_DIRS} ${TINY_DNN_INCLUDE})
  include_directories("../libdigit_recog")
  link_directories(${JAVA_JVM_LIBRARY})

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
  
  file(GLOB JNI_WRAPPER_SOURCES "*.cpp" "*.h" "*.hpp")
  
  add_library(libjni_wrapper SHARED ${JNI_WRAPPER_SOURCES})

  target_link_libraries(libjni_wrapper ${JNI_LIBRARIES} libdigit_recog)
endif()

