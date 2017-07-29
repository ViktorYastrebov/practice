#include "JNIDigitRecog.h"

//#include <iostream>
#include <fstream>

#include <vector>

#include <codecvt>
#include <locale>

#include "digit_recognition.h"

#include "tiny_dnn/util/util.h"

JNIEXPORT jint JNICALL Java_JNIDigitRecog_predict(JNIEnv *env, jobject obj, jstring str) {
  const char  *cpath = env->GetStringUTFChars(str, NULL);
  digit_recognition::digit_recognizer recognizer;

  /*std::ofstream ofs("log.out");
  if (ofs) {
    ofs << "path = " << cpath << std::endl;
  }*/

  jint predict = -1;
  try {
    std::string path(cpath);
    tiny_dnn::vec_t data;
    recognizer.convert_image(path, data);
    std::string modelPath("d:\\programming_workspace\\c++\\cpp_ml\\tiny-dnn\\models\\mnist\\model_net");
    recognizer.getNN().load(modelPath);
    predict = recognizer.predict(data);
  }
  catch (const std::exception  &ex) {
    //ofs << "exception occurs:" << ex.what() << std::endl;
    const char rtName[] = { "java/lang/RuntimeException" };
    std::string excepDesc("C++ side exception: ");
    excepDesc += std::string(ex.what());
    jclass rtexcep = env->FindClass(rtName);
    env->ThrowNew(rtexcep, excepDesc.c_str());
  }
  env->ReleaseStringUTFChars(str, cpath);
  return predict;
}
