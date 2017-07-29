#ifndef JNI_COMMON_H
#define JNI_COMMON_H

#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4800 ) /*forcing value to bool 'true' or 'false' (performance warning)*/
#endif

#include "../Syb/SybAll.h"
#include "boost/date_time/gregorian/gregorian.hpp"
#include <string>

class _jobject;
typedef _jobject *jobject;

struct JNIEnv_;
typedef JNIEnv_ JNIEnv;

class _jclass;
typedef _jclass *jclass;

struct _jmethodID;
typedef struct _jmethodID *jmethodID;

union jvalue;

typedef long          jint__;
typedef unsigned char	jboolean__;
typedef double		    jdouble__;
typedef signed char   jbyte__;

template<typename T>
inline T* checkPtr(T *p) {
  if (!p) throw std::runtime_error("couldn't create an object");
  return p;
}

template<typename T>
inline T* checkPtr(T *p, const std::string& mes) {
  if (!p) throw std::runtime_error(mes);
  return p;
}

namespace qx {
  struct BinaryData;

  /*template <class T>
  struct JniCallMethodTypeStruct {
    typedef T (JNIEnv::*type)(jobject jobj, jmethodID methodID, va_list args);
  };*/  
  const std::string& getJavaPackage();
  
  template <typename RetType>
  struct BoostFunction {
    typedef boost::function<RetType (JNIEnv*, jobject jobj, jmethodID methodID, jvalue* args)> type;
  };    

  namespace syb {
    namespace priv {      

      template<class T, class EnableT = void>
      struct JavaTypeTraits {

        static inline std::string simpleValue() {
          static std::string ret = TypeName<T>::value();
          return ret;
        }

        static inline const std::string& name() {
          static std::string ret = std::string("L") + simpleName() + ";"; 
          return ret;
        } 
        static inline const std::string& simpleName() {
          static std::string ret = getJavaPackage() + TypeName<T>::value();
          return ret;
        }
        static inline const std::string& constrSignature() {
          static std::string ret = "()V";
          return ret;
        }
      };      

      template<class T>
      struct JavaTypeTraits<T, typename boost::enable_if<IsCont<T> >::type> {
        static inline std::string simpleValue() {
          static std::string ret = TypeName<T>::value();
          return ret;
        }

        static inline const std::string& name() {
          static std::string ret = std::string("L") + simpleName() + ";";
          return ret;
        }
        static inline const std::string& simpleName() {
          static std::string ret = JavaTypeTraits< typename UnAlias<typename T::value_type>::type >::simpleName();
          return ret;
        }
        static inline const std::string& constrSignature() {
          static std::string ret = "()V";
          return ret;
        }
      };

      template<class T1, class T2>
      struct JavaTypeTraits<std::pair<T1,T2>,
        typename boost::enable_if <                      
            typename boost::mpl::not_ <
              boost::is_same<
                typename UnAlias<T1>::type, 
                std::string
              >
            >::type  
        >::type 
      > {

        static inline const std::string& name() {
          static std::string ret = std::string("L") + simpleName() + ";";
          return ret;
        }
        static inline const std::string& simpleName() {
          static std::string ret = "com/querix/utils/Pair";
          return ret;
        }
        static inline const std::string& constrSignature() {
          static std::string ret = "()V";
          return ret;
        }
      };
      
      template<class T1, class T2>
      struct JavaTypeTraits<std::pair<T1, T2>, 
        typename boost::enable_if <
          typename boost::mpl::and_ <
            boost::is_same< 
              typename UnAlias<T1>::type, 
              std::string 
            >
            ,boost::is_same< 
              typename UnAlias<T2>::type, 
              std::string
            >
          >::type
        >::type > {

        static inline const std::string& name() {
          static std::string ret = std::string("L") + simpleName() + ";";
          return ret;
        }
        static inline const std::string& simpleName() {
          static std::string ret = "com/querix/utils/StringPair";
          return ret;
        }
        static inline const std::string& constrSignature() {
          static std::string ret = "()V";
          return ret;
        }
      };

      template<class T>
      struct JavaTypeTraits<boost::optional<T> > {        
        static inline std::string simpleValue() {
          static std::string ret = TypeName<T>::value();
          return ret;
        }

        static inline const std::string& name() {
          static std::string ret = std::string("L") + simpleName() + ";";
          return ret;
        }
        static inline const std::string& simpleName() {
          static std::string ret = JavaTypeTraits<typename UnAlias<T>::type>::simpleName();
          return ret;
        }        
      };

      template<>
      struct JavaTypeTraits<int> {

        static BoostFunction<jint__>::type getCallFunctor();

        static inline std::string simpleValue() {
          static std::string ret = "int";
          return ret;
        }
        static inline const std::string& name() {
          static std::string ret = std::string("L") + simpleName() + ";";
          return ret;
        }
        static inline const std::string& simpleName() {
          static std::string ret = "java/lang/Integer";
          return ret;
        }
        static inline const std::string& typeDigit() {
          static std::string ret = "I";
          return ret;
        }
        static inline const std::string& constrSignature() {
          static std::string ret = std::string("(") + typeDigit() +")V" ;
          return ret;
        }
      };

      template<>
      struct JavaTypeTraits<unsigned char> {

        static BoostFunction<jbyte__>::type getCallFunctor();

        static inline const std::string& simpleValue() {
          static std::string ret = "byte";
          return ret;
        }
        static inline const std::string& name() {
          static std::string ret = std::string("L") + simpleName() + ";";
          return ret;
        }
        static inline const std::string& simpleName() {
          static std::string ret = "java/lang/Byte";
          return ret;
        }
        static inline const std::string& typeDigit() {
          static std::string ret = "B";
          return ret;
        }
        static inline const std::string& constrSignature() {
          static std::string ret = std::string("(") + typeDigit() +")V" ;
          return ret;
        }
      };

      template<>
      struct JavaTypeTraits<bool> {

        static BoostFunction<jboolean__>::type getCallFunctor();

        static inline std::string simpleValue() {
          static std::string ret = "boolean";
          return ret;
        }
        static inline const std::string& name() {
          static std::string ret = std::string("L") + simpleName() + ";";
          return ret;
        }
        static inline const std::string& simpleName() {
          static std::string ret = "java/lang/Boolean";
          return ret;
        }
        static inline const std::string& typeDigit() {
          static std::string ret = "Z";
          return ret;
        }
        static inline const std::string& constrSignature() {
          static std::string ret = std::string("(") + typeDigit() +")V" ;
          return ret;
        }
      };

      template<>
      struct JavaTypeTraits<double> {

        static BoostFunction<jdouble__>::type getCallFunctor();

        static inline std::string simpleValue() {
          static std::string ret = "double";
          return ret;
        }
        static inline const std::string& name() {
          static std::string ret = std::string("L") + simpleName() + ";";
          return ret;
        }
        static inline const std::string& simpleName() {
          static std::string ret = "java/lang/Double";
          return ret;
        }
        static inline const std::string& typeDigit() {
          static std::string ret = "D";
          return ret;
        }
        static inline const std::string& constrSignature() {
          static std::string ret = std::string("(") + typeDigit() +")V" ;
          return ret;
        }
      };

      template<>
      struct JavaTypeTraits<boost::gregorian::date> {

        static inline const std::string& name() {
          static std::string ret = std::string("L") + simpleName() + ";";
          return ret;
        }
        static inline const std::string& simpleName() {
          static std::string ret = "java/util/Date";
          return ret;
        }
        static inline const std::string& constrSignature() {
          static std::string ret = std::string("(") + "III" +")V" ;
          return ret;
        }
      };

      template<>
      struct JavaTypeTraits<std::string> {

        static inline const std::string& simpleValue() {
          static std::string ret = "String";
          return ret;
        }
        static inline const std::string& name() {
          static std::string ret = std::string("L") + simpleName() + ";";
          return ret;
        }
        static inline const std::string& simpleName() {
          static std::string ret = "java/lang/String";
          return ret;
        }
        static inline const std::string& typeDigit() {
          static std::string ret = "S";
          return ret;
        }
        static inline const std::string& constrSignature() {
          static std::string ret = std::string("(") + typeDigit() +")V" ;
          return ret;
        }
      };

      template<>
      struct JavaTypeTraits<UnicodeString> {

        static inline const std::string& simpleValue() {
          static std::string ret = "String";
          return ret;
        }
        static inline const std::string& name() {
          static std::string ret = std::string("L") + simpleName() + ";";
          return ret;
        }
        static inline const std::string& simpleName() {
          static std::string ret = "java/lang/String";
          return ret;
        }
        static inline const std::string& typeDigit() {
          static std::string ret = "S";
          return ret;
        }
        static inline const std::string& constrSignature() {
          static std::string ret = std::string("(") + typeDigit() +")V" ;
          return ret;
        }
      };      
    }
  }
}

#endif /*#ifndef JNI_COMMON_H*/
