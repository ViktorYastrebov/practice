#include "JniCommon.h"

#include "boost/bind.hpp"
#include <boost/function.hpp>
#include <jni.h>


namespace qx {
  namespace syb {
    namespace priv {   

      BoostFunction<jint__>::type JavaTypeTraits<int>::getCallFunctor() {
        static BoostFunction<jint__>::type f = boost::bind(&JNIEnv::CallIntMethodA, _1, _2, _3, _4);        
        return f;
      }      

      BoostFunction<jbyte__>::type JavaTypeTraits<unsigned char>::getCallFunctor(){        
        static BoostFunction<jbyte__>::type f = boost::bind(&JNIEnv::CallByteMethodA, _1, _2, _3, _4);        
        return f;
      }

      BoostFunction<jboolean__>::type JavaTypeTraits<bool>::getCallFunctor() {        
        static BoostFunction<jboolean__>::type f = boost::bind(&JNIEnv::CallBooleanMethodA, _1, _2, _3, _4);        
        return f;
      }

      BoostFunction<jdouble__>::type JavaTypeTraits<double>::getCallFunctor() {        
        static BoostFunction<jdouble__>::type f = boost::bind(&JNIEnv::CallDoubleMethodA, _1, _2, _3, _4);        
        return f;
      }
    }
  }
}

