#include "JniReader.h"

#include <jni.h>
#include <boost/shared_ptr.hpp>
#include <cstdarg>
#include <stack>

using namespace qx;
using namespace qx::syb;

namespace Loki {
template<>
struct ImplOf<qx::syb::priv::JniReaderImpl> {
  ImplOf() : index(-1) {}
  ~ImplOf() {}

  mutable std::stack<jobject> jniObjects;
  int index;
  JNIEnv *je;
  jclass jc;
};
}

namespace qx {
  namespace syb {
    namespace priv {

      JniReaderImpl::JniReaderImpl() : fieldNameToUse_("") {
        TraceOption<qx::syb::priv::JniReaderImpl>::instance().enable(false);
      }

      JniReaderImpl::~JniReaderImpl(){}

      void JniReaderImpl::getJavaPrimitiveObjectVal(UnicodeString &v, jobject jobj) const {
        std::string str;
        getJavaPrimitiveObjectVal(str, jobj);
        v = UnicodeString(str.c_str());
      }

      void JniReaderImpl::getJavaPrimitiveObjectVal(std::string &v, jobject jobj) const {
        const char *tmpPtr = impl_->je->GetStringUTFChars((jstring)jobj, NULL);
        v = tmpPtr;
        impl_->je->ReleaseStringUTFChars((jstring)jobj, tmpPtr);
      }            

      void JniReaderImpl::getJavaPrimitiveObjectVal(boost::gregorian::date& v, jobject jobj) const {
        int year = 0, month = 0, day = 0;

        const std::string getYearMethodName = "getYear";
        const std::string getMonthMethodName = "getMonth";
        const std::string getDayMethodName = "getDate";
        const std::string methodSignature = "()" + JavaTypeTraits<int>::typeDigit();
          
        getJavaObjectVal(jobj, getYearMethodName, methodSignature, year);
        year += 1900;

        getJavaObjectVal(jobj, getMonthMethodName, methodSignature, month);
        month +=1;

        getJavaObjectVal(jobj, getDayMethodName, methodSignature, day);       

        v = boost::gregorian::date(year, month, day);
      }

      void JniReaderImpl::getMethodId(
              const jobject& jobj, 
              const std::string &methodName, 
              const std::string &methodSignature, 
              jmethodID& jm) const {

        jclass jobjClass = checkPtr(impl_->je->GetObjectClass(jobj));
        jm = checkPtr(impl_->je->GetMethodID(jobjClass, methodName.c_str(), methodSignature.c_str()));
      }

      JNIEnv* JniReaderImpl::getJniEnv() const {
        return impl_->je;
      }

      jobject JniReaderImpl::callParentGetter(
              jobject jParentObj,
              const std::string &typeNameProposed,
              const std::string &javaUnAliasTypeName,
              bool isCont) {

        jobject result = NULL;
        std::string typeName = typeNameProposed;

        if (fieldNameToUse() != "") {
          typeName = fieldNameToUse();
          fieldNameToUse("");
        }

        if (getIndex() != -1) {
          result = checkPtr(impl_->je->GetObjectArrayElement(static_cast<jobjectArray>(jParentObj), getIndex()));
          setIndex(-1);
        } else {

          std::string methodName = "get";
          std::string methodSignature = "()";

          if (isCont) {
            methodName += typeName + "Arr";
            methodSignature += "[";
          } else {
            methodName += typeName;
          }
          
          //methodSignature += "L" + javaUnAliasTypeName + ";";
          methodSignature += javaUnAliasTypeName;

          jclass jParentCl = checkPtr(impl_->je->GetObjectClass(jParentObj));

          jmethodID jm = checkPtr(impl_->je->GetMethodID(jParentCl, methodName.c_str(), methodSignature.c_str()));

          result = impl_->je->CallObjectMethod(jParentObj, jm);
        }



        return result;
      }

      void JniReaderImpl::constructTypeAfter() {
        if (getSize() > 1) {          
          popItem();
        }
      }

      int JniReaderImpl::getIndex() const {return impl_->index;}

      void JniReaderImpl::setIndex(int index){impl_->index = index;}

      void JniReaderImpl::pushItem(jobject jobj) {
        impl_->jniObjects.push(jobj);
      }

      jobject JniReaderImpl::topItem() const {
        return impl_->jniObjects.top();
      }

      void JniReaderImpl::popItem() {
        impl_->jniObjects.pop();
      }

      bool JniReaderImpl::isEmpty() const {
        return impl_->jniObjects.empty();
      }

      int JniReaderImpl::getSize() const {
        return impl_->jniObjects.size();
      }

      int JniReaderImpl::getArrayLen(jobject jobjArr) const {
        
        if (jobjArr) {
          return impl_->je->GetArrayLength(static_cast<jobjectArray>(jobjArr));
        } else {
          return 0;
        }
      }

      unsigned char* JniReaderImpl::getByteArrayElements(jobject jobjArr) {
       
        if (jobjArr) {
          return reinterpret_cast<unsigned char*>(
            impl_->je->GetByteArrayElements(static_cast<jbyteArray>(jobjArr), false));
        } else {
          return NULL;
        }
      }

      void JniReaderImpl::setJniEnv(JNIEnv *je, jclass jc, jobject jrootObj) {
        impl_->je = je;
        impl_->jc = jc;
        pushItem(jrootObj);
      }

      std::string JniReaderImpl::jniPolyName(jobject object) {
        // get object class
        jclass cls = checkPtr(impl_->je->GetObjectClass(object));
        jmethodID method =
          checkPtr(impl_->je->GetMethodID(cls, "getClass", "()Ljava/lang/Class;"));
        jobject class_object = checkPtr(impl_->je->CallObjectMethod(object, method));

        // get class name
        cls = checkPtr(impl_->je->GetObjectClass(class_object));
        method = checkPtr(impl_->je->GetMethodID(cls,"getName","()Ljava/lang/String;"));
        jstring jPolyName = checkPtr(static_cast<jstring>(impl_->je->CallObjectMethod(class_object,method)));

        std::string polyName;
        getJavaPrimitiveObjectVal(polyName, static_cast<jobject>(jPolyName));

        size_t pos = polyName.find_last_of('.');
        if(pos != std::string::npos)
          polyName.erase(0, pos + 1);
        return polyName;
      }

    }
  }
}
