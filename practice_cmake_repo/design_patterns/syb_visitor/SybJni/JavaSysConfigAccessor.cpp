#include "JavaSysConfigAccessor.h"
#include <jni.h>

bool qx::JavaSysConfigAccessor::getProperty(JNIEnv *je, const std::string propName, std::string& res) {
  jclass jSystemClass = checkPtr(je->FindClass("java/lang/System"), "java.lang.System has not been found");
  jmethodID jm = checkPtr(je->GetStaticMethodID(jSystemClass, "getProperty", "(Ljava/lang/String;)Ljava/lang/String;"), 
      "java.lang.System.getProperty has not been found");

  jstring prop = checkPtr(je->NewStringUTF(propName.c_str()));

  jstring result = (jstring)je->CallStaticObjectMethod(jSystemClass, jm, prop);
  if (result) {
    const char *tmpPtr = checkPtr(je->GetStringUTFChars((jstring)result, NULL));
    res = tmpPtr;
    je->ReleaseStringUTFChars((jstring)result, tmpPtr);
    return true;
  }

  return false;
}
