#ifndef JNI_READER_H
#define JNI_READER_H

#include "JniCommon.h"

#include "../Syb/SybAll.h"
#include "../Syb/Binary.h"
#include "../Syb/Base32.h"
#include "../Syb/Any.h"

#include <loki/Pimpl.h>
#include <boost/lexical_cast.hpp>
#include <cstdarg>


namespace qx {   
  namespace syb {

    template <class TagT>
    struct JniReader;

    namespace priv {
      template<typename T, typename TagT, typename EnableT=void>
      struct JniReaderDispatcher;

      struct JniReaderImpl {

        explicit JniReaderImpl();

        ~JniReaderImpl();
        
        void getMethodId(
                const jobject& jobj, 
                const std::string &methodName, 
                const std::string &methodSignature, 
                jmethodID& jm) const;
        
        JNIEnv* getJniEnv() const; 

        template <typename T>
        inline void getJavaObjectVal(
                const jobject& jobj, 
                const std::string &methodName, 
                const std::string &methodSignature, 
                T &v) const {        
          
          jmethodID jm;
          getMethodId(jobj, methodName, methodSignature, jm); 
                              
          v = JavaTypeTraits<T>::getCallFunctor()(getJniEnv(), jobj, jm, NULL);
        }

        template <typename T>
        void getJavaPrimitiveObjectVal(T &v, jobject jobj) const {
          std::string methodName = JavaTypeTraits<T>::simpleValue() + "Value";
          std::string methodSignature = "()" + JavaTypeTraits<T>::typeDigit();                   

          getJavaObjectVal(jobj, methodName, methodSignature, const_cast<T &>(v));
        }

        void getJavaPrimitiveObjectVal(boost::gregorian::date& v, jobject jobj) const;
        void getJavaPrimitiveObjectVal(UnicodeString &v, jobject jobj) const;
        void getJavaPrimitiveObjectVal(std::string &v, jobject jobj) const;                                    
       
        jobject callParentGetter(
                jobject jParentObj,
                const std::string &typeName,
                const std::string &javaUnAliasTypeName,
                bool isCont = false);

        template<typename T>
        void constructPrimitive(T &v) {

          jobject jobj;
          if (getSize() > 1) {
            jobj = /*checkPtr(*/callParentGetter(
              topItem(),
              //TypeName<T>::value(),
              JavaTypeTraits<T>::simpleValue(),
              JavaTypeTraits<typename UnAlias<T>::type>::name());
          } else {
            jobj = topItem();
          }

          if (jobj) {            
            getJavaPrimitiveObjectVal(unAlias(v), jobj);                        
          } else {
            throw std::invalid_argument(std::string(JavaTypeTraits<T>::simpleValue()) + " field should not be empty");
          }
        }

        //TODO: check this!
        template<typename T>
        void constructOptional(T &v) {

          jobject jobj = /*checkPtr(*/callParentGetter(
              topItem(),
              TypeName<T>::value(),
              JavaTypeTraits<typename UnAlias<T>::type>::name());

          if (jobj) {
            getJavaPrimitiveObjectVal(unAlias(v).get(), jobj);
          }
        }
        
        template<typename T>
        bool constructTypeBefore(T const& v) {
          bool res = true;

          if (getSize() > 1) {

            jobject jobj = /*checkPtr(*/callParentGetter(
                topItem(),
                TypeName<T>::value(),
                JavaTypeTraits<typename UnAlias<T>::type>::name());

            if (jobj) {
              pushItem(jobj);
            } else {
              res = false;
            }
          } else {                        

            pushItem(topItem());
          }

          return res;
        }

        void constructTypeAfter();        

        template<typename T>
        bool constructPolyTypeBefore(T &v) {
          if(getSize() > 1) {
            jobject polyObj = callParentGetter(
              topItem(),
              TypeName<T>::value(),
              JavaTypeTraits<typename UnAlias<T>::type>::name());
            if(polyObj) {
              DataType<typename UnAlias<T>::type> const &dt
                = dataType<typename UnAlias<T>::type>();
              std::string const &actualName = jniPolyName(polyObj);
              dt.dynByName(actualName)->dynBuild(toAny(v));
              pushItem(polyObj);
              return true;
            }
          }
          return false;
        }

        void constructPolyTypeAfter() {
          constructTypeAfter();
        };

        void pushItem(jobject jobj);

        jobject topItem() const;

        void popItem();

        bool isEmpty() const;

        int getSize() const;

        int getArrayLen(jobject jobjArr) const;

        unsigned char* getByteArrayElements(jobject jobjArr);

        void setIndex(int index);

        int getIndex() const;

        void setJniEnv(JNIEnv *je, jclass jc, jobject jrootObj);

        std::string jniPolyName(jobject object);

        std::string fieldNameToUse() {return fieldNameToUse_;}
        void fieldNameToUse(const std::string& fieldNameToUse_) {this->fieldNameToUse_ = fieldNameToUse_;}
      private:
        std::string fieldNameToUse_;
        Loki::PimplOf<JniReaderImpl>::Type impl_;
      }; 
    
      /*---------------------Dispatchers------------------------------*/    
      template<typename T,typename TagT, typename EnableT>
      struct JniReaderDispatcher {
        inline static void
        action(JniReader<TagT> const& w, T & v) {
          TRACE_SCOPE(JniReaderImpl, "action", JavaTypeTraits<T>::simpleValue());

          if (w.impl_.constructTypeBefore(v)) {
            gmap(unAlias(v),w,Unit());
            w.impl_.constructTypeAfter();
          }
        }
      };
      
      template<typename T,typename TagT>
      struct JniReaderDispatcher<T, TagT, 
        typename boost::enable_if <
          boost::is_same< 
            typename UnAlias<T>::type, 
            boost::optional<typename UnAlias<T>::type::value_type> 
          >
        >::type
      > {
        inline static void action(JniReader<TagT> const& w, T& v) {
          TRACE_SCOPE(JniReaderImpl, "action", JavaTypeTraits<T>::simpleValue());

          w.impl_.fieldNameToUse(JavaTypeTraits<T>::simpleValue());

          typename UnAlias<T>::type::value_type val;

          try {
            w(val);
            unAlias(v) = val;
          } catch (std::invalid_argument& ) {
          }

          w.impl_.fieldNameToUse("");
        }
      };


      template<typename T,typename TagT>
      struct JniReaderDispatcher<T, TagT, 
        typename boost::enable_if <
          IsEnum<typename UnAlias<T>::type> 
        >::type 
      > {
        inline static void
        action(JniReader<TagT> const& w, T& v) {
          TRACE_SCOPE(JniReaderImpl, "action", JavaTypeTraits<T>::simpleValue());

          jobject jobj;
          if (w.impl_.getSize() > 1) {
            jobj = /*checkPtr(*/w.impl_.callParentGetter(
              w.impl_.topItem(),
              TypeName<T>::value(),
              JavaTypeTraits<int>::name());
          } else {
            jobj = w.impl_.topItem();
          }
           
          if (jobj) {
            int val;
            w.impl_.getJavaPrimitiveObjectVal(val, jobj);            
            unAlias(v) = val;
          }
        }
      };


      template<typename T,typename TagT>
      struct JniReaderDispatcher<T, TagT, 
        typename boost::enable_if <
          IsPrimitive<typename UnAlias<T>::type> 
        >::type 
      > {
        inline static void action(JniReader<TagT> const& w, T& v) {
           TRACE_SCOPE(JniReaderImpl, "action", JavaTypeTraits<T>::simpleValue());

           w.impl_.constructPrimitive(v);
        }
      };
      
      /*template<typename T,typename TagT>
      struct JniReaderDispatcher<T, TagT, 
        typename boost::enable_if <
          boost::is_same< 
            typename UnAlias<T>::type, 
            lcm::TrueFalseBool 
          >
        >::type 
      > {
        inline static typename void action(JniReader<TagT> const& w, T& v) {
           TRACE_SCOPE(JniReaderImpl, "action", JavaTypeTraits<T>::simpleValue());

           w.impl_.fieldNameToUse(JavaTypeTraits<T>::simpleValue());
           w(unAlias(v));
           w.impl_.fieldNameToUse("");
        }
      };*/


      template<typename T,typename TagT>
      struct JniReaderDispatcher<T, TagT,
        typename boost::enable_if <
          typename boost::mpl::and_ <
            IsCont<typename UnAlias<T>::type>,
            typename boost::mpl::not_ <
              boost::is_same< 
                typename UnAlias<T>::type, 
                Binary
              >
            >::type
          >::type 
        >::type 
      > {
        inline static void action(JniReader<TagT> const& w, T& v) {
          TRACE_SCOPE(JniReaderImpl, "action", JavaTypeTraits<T>::simpleValue());

          typedef typename UnAlias<T>::type::value_type val_t;
          typedef typename UnAlias<val_t>::type cont_elem_type;
          
          std::string javaTypeName = JavaTypeTraits<cont_elem_type>::name();
          
          jobject jobjArr = /*checkPtr(*/w.impl_.callParentGetter(
            w.impl_.topItem(),
            TypeName<T>::value(),
            JavaTypeTraits<typename UnAlias<T>::type>::name(), true);

          if (jobjArr) {
            w.impl_.pushItem(jobjArr);

            int len = w.impl_.getArrayLen(jobjArr);

            for (int i = 0; i < len ; ++i) {
              w.impl_.setIndex(i);
            
              unAlias(v).push_back(val_t());

              w(unAlias(v).back());
            }

            w.impl_.popItem();
          }
        }
      };


      template<typename T,typename TagT>
      struct JniReaderDispatcher<T, TagT, 
        typename boost::enable_if <
          boost::is_same< 
            typename UnAlias<T>::type, 
            Binary
          > 
        >::type
      > {
        inline static void
        action(JniReader<TagT> const& w, T& v) {          
          TRACE_SCOPE(JniReaderImpl, "action", JavaTypeTraits<T>::simpleValue());

          jobject jobjArr = w.impl_.callParentGetter(
              w.impl_.topItem(),
              "Binary", 
              JavaTypeTraits<unsigned char>::typeDigit(), true);                    

          int len = w.impl_.getArrayLen(jobjArr);
          unsigned char *bytes = w.impl_.getByteArrayElements(jobjArr);

          v.assign(bytes, bytes + len);          
        }
      };

      template<typename T, typename TagT>
      struct JniReaderDispatcher<T, TagT, typename boost::enable_if<mpl::and_<
            NotOpt<typename UnAlias<T>::type>,
            IsMultiConstr<T>
          > >::type> {
        inline static void action(JniReader<TagT> const &r, T &v) {
          TRACE_SCOPE(JniReaderImpl, "action", JavaTypeTraits<T>::simpleValue());

          if(r.impl_.constructPolyTypeBefore(v)) {
            gmap(v, r, Unit());
            r.impl_.constructPolyTypeAfter();
          }
        }
      };

      /*template<typename TagT>
      struct JniReaderDispatcher<qx::BinaryData, TagT, void> {
        inline static typename void action(JniReader<TagT> const& w, qx::BinaryData& v) {

          jobject jobj = checkPtr(w.impl_.callParentGetter(
            w.impl_.topItem(),
            TypeName<qx::BinaryData>::value(),
            JavaTypeTraits<qx::BinaryData>::name(), false));

          w.impl_.pushItem(jobj);

          jobject jobjArr = checkPtr(w.impl_.callParentGetter(
              w.impl_.topItem(),
              "Bytes", 
              JavaTypeTraits<unsigned char>::typeDigit(), true));
          
          w.impl_.pushItem(jobjArr);

          int len = w.impl_.getArrayLen(jobjArr);
          unsigned char *bytes = w.impl_.getByteArrayElements(jobjArr);

          v.val().assign(bytes, bytes + len);

          w.impl_.popItem();
          w.impl_.popItem();
        }
      };*/      
    }

    /*------------------------------------------------------------------*/      
    template<typename TagT>
    struct JniReader:Visitor<JniReader<TagT> > {
      inline explicit JniReader(JNIEnv *je, jclass jc, jobject jrootObj) {
        assert(je);
        assert(jc);
        assert(jrootObj);
        impl_.setJniEnv(je, jc, jrootObj);
      }

      template<typename T>
      inline void operator()(T& v) const {
        priv::JniReaderDispatcher<T,TagT>::action(*this,v);
      }

    private:
      mutable priv::JniReaderImpl impl_;

      template<typename T, typename TagU, typename EnableT>
      friend struct priv::JniReaderDispatcher;
    };
  }
}

#endif /*JNI_READER_H*/
