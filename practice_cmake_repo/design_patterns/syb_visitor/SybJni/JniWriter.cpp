#include "JniWriter.h"

#include <boost/shared_ptr.hpp>
#include <jni.h>
#include <cstdarg>

#include <stack>

using namespace qx;
using namespace qx::syb;

namespace Loki {
template<>
struct ImplOf<qx::syb::priv::JniWriterImpl> {
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

      JniWriterImpl::JniWriterImpl() : fieldNameToUse_("") {
        TraceOption<qx::syb::priv::JniWriterImpl>::instance().enable(false);
      }

      JniWriterImpl::~JniWriterImpl() {}

      void JniWriterImpl::callParentSetter(
          jobject jParentObj,
          const std::string& childTypeNameProposed,
          const std::string& childJavaUnAliasTypeName,
          jobject jChildObj, bool isCont) {

        std::string childTypeName = childTypeNameProposed;

        if (fieldNameToUse() != "") {
          childTypeName = fieldNameToUse();
          fieldNameToUse("");
        }

        if (getIndex() != -1) {
          //this means that its parent is array!
          impl_->je->SetObjectArrayElement(static_cast<jobjectArray>(jParentObj), getIndex(), jChildObj);
          setIndex(-1);
        } else {
          std::string methodName = "set";
          std::string methodSignature = "(";

          if (isCont) {
            methodName += childTypeName + "Arr";
            methodSignature += "[";
          } else {
            methodName += childTypeName;
          }

          //methodSignature += "L" + childJavaUnAliasTypeName + ";)V";
          methodSignature += childJavaUnAliasTypeName + ")V";

          jclass jParentCl = impl_->je->GetObjectClass(jParentObj);

          jmethodID jm = checkPtr(
            impl_->je->GetMethodID(jParentCl, methodName.c_str(), methodSignature.c_str()));

          impl_->je->CallVoidMethod(jParentObj, jm, jChildObj);
        }
      }

      int JniWriterImpl::getIndex() const {return impl_->index;}

      void JniWriterImpl::setIndex(int index){impl_->index = index;}

      jobject JniWriterImpl::createArray(const std::string& javaTypeName, int len) const {
        jclass jc = impl_->je->FindClass(javaTypeName.c_str());
        return impl_->je->NewObjectArray(len, jc, NULL/*ji*/);
      }

      jobject JniWriterImpl::createByteArray(int len) const {
        jbyteArray jbyteArr = impl_->je->NewByteArray(len);
        return jbyteArr;
      }

      void JniWriterImpl::setByteArrayRegion(
            jobject jobjArr, int start, int len, const unsigned char *buf) const {
        impl_->je->SetByteArrayRegion(
          static_cast<jbyteArray>(jobjArr), start, len, reinterpret_cast<const jbyte*>(buf));
      }

      void JniWriterImpl::pushItem(jobject jobj) {
        impl_->jniObjects.push(jobj);
      }

      void JniWriterImpl::popItem() {
        impl_->jniObjects.pop();
      }

      jobject& JniWriterImpl::topItem() const {
        return impl_->jniObjects.top();
      }

      bool JniWriterImpl::isEmpty() const {
        return impl_->jniObjects.empty();
      }

      int JniWriterImpl::getSize() const {
        return impl_->jniObjects.size();
      }      

      jobject JniWriterImpl::createJavaObject(const bool& v) const {
        const std::string &javaTypeName = JavaTypeTraits<bool>::name();
        const std::string &sign = JavaTypeTraits<bool>::constrSignature();
        jvalue jv;
        jv.z = unAlias(v);
        jvalue *pjv = &jv;
        return createJavaObjectAux(javaTypeName, sign, pjv);
      }

      jobject JniWriterImpl::createJavaObject(const double& v) const {
        const std::string &javaTypeName = JavaTypeTraits<double>::name();
        const std::string &sign = JavaTypeTraits<double>::constrSignature();
        jvalue jv;
        jv.d = unAlias(v);
        jvalue *pjv = &jv;
        return createJavaObjectAux(javaTypeName, sign, pjv);
      }

      jobject JniWriterImpl::createJavaObject(const int& v) const {
        const std::string &javaTypeName = JavaTypeTraits<int>::name();
        const std::string &sign = JavaTypeTraits<int>::constrSignature();
        jvalue jv;
        jv.i = unAlias(v);
        jvalue *pjv = &jv;
        return createJavaObjectAux(javaTypeName, sign, pjv);
      }

      jobject JniWriterImpl::createJavaObject(const std::string& v) const {
        jobject jStringObj = impl_->je->NewStringUTF(v.c_str());
        return jStringObj;
      }

      jobject JniWriterImpl::createJavaObject(const UnicodeString& v) const {
        std::string str;
        v.toUTF8String(str);
        return createJavaObject(str);
      }

      jobject JniWriterImpl::createJavaObject(const boost::gregorian::date& v) const {

        const std::string &javaTypeName = JavaTypeTraits<boost::gregorian::date>::name();
        const std::string &sign = JavaTypeTraits<boost::gregorian::date>::constrSignature();

        jvalue jdate[3];

        if (!v.is_not_a_date()) {
          const int javaYearStart = 1900;
          jdate[0].i = v.year() - javaYearStart;
          jdate[1].i = v.month() - 1;
          jdate[2].i = v.day();
        }
        jvalue *pjdate = &jdate[0];
        return createJavaObjectAux(javaTypeName, sign, pjdate);
      }
      
      jobject JniWriterImpl::createJavaObjectAux(
          const std::string& javaTypeName,
          const std::string& sign, jvalue *args) const {

        jobject result = NULL;
        jclass jc = NULL;
        jc = impl_->je->FindClass(javaTypeName.c_str());

        checkPtr(jc);
        jmethodID jm = checkPtr(impl_->je->GetMethodID(jc, "<init>", sign.c_str()));

        result = impl_->je->NewObjectA(jc, jm, args);

        return result;
      }

      void JniWriterImpl::constructTypeAfter() {
        if (getSize() > 1) {
          popItem();
        }
      }

      void JniWriterImpl::setJniEnv(JNIEnv *je, jclass jc, jobject& jrootObj) {
        impl_->je = je;
        impl_->jc = jc;
        pushItem(jrootObj);
      }
    }
  }
}
