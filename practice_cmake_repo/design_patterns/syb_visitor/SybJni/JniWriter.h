#ifndef JNI_WRITTER_H
#define JNI_WRITTER_H

#include "JniCommon.h"

#include "../Syb/SybAll.h"
#include "../Syb/Binary.h"
#include "../Syb/Base32.h"
#include "../Syb/Any.h"

#include <boost/lexical_cast.hpp>
#include <loki/Pimpl.h>

//#include <stack>

namespace qx {
  namespace crypto {
    struct HashType;
  }
}

namespace qx {
  namespace syb {

    template<typename TagT>
    struct JniWriter;

    namespace priv {
      template<typename T, typename TagT, typename EnableT=void>
      struct JniWriterDispatcher;

      struct JniWriterImpl {

        explicit JniWriterImpl();

        ~JniWriterImpl();

        template<typename T>
        jobject createJavaObject(const T& v) const {
          const std::string& javaTypeName = JavaTypeTraits<typename UnAlias<T>::type>::name();
          const std::string& sign = JavaTypeTraits<typename UnAlias<T>::type>::constrSignature();
          //jvalue jv;
          //jv.l = unAlias(v);
          //jvalue *pjv = &jv;
          return createJavaObjectAux(javaTypeName, sign, NULL);
        }

        jobject createJavaObject(const bool& v) const;

        jobject createJavaObject(const double& v) const;

        jobject createJavaObject(const int& v) const;

        jobject createJavaObject(const std::string& v) const;

        jobject createJavaObject(const UnicodeString& v) const;

        jobject createJavaObject(const boost::gregorian::date& v) const;
        
        jobject createJavaObjectAux(
            const std::string& javaTypeName,
            const std::string& sign, jvalue *args) const;

        void callParentSetter(
            jobject jParentObj,
            const std::string& childTypeName,
            const std::string& childJavaUnAliasTypeName,
            jobject jChildObj, bool isCont = false);

        template<typename T>
        void constructPrimitive(const T& v) {

          jobject jobj = checkPtr(createJavaObject(unAlias(v)));

          if (topItem() != NULL) {
            callParentSetter(
              topItem(),
              //TypeName<T>::value(),
              JavaTypeTraits<T>::simpleValue(),
              JavaTypeTraits<typename UnAlias<T>::type>::name(),
              jobj);
          } else {
            //this means it has not been set yet
            topItem() = jobj;
          }
        }

        template<typename T>
        void constructOptional(const T& v) {

          jobject result = checkPtr(createJavaObject(unAlias(v).get()));

          callParentSetter(
            topItem(),
            TypeName<T>::value(),
            JavaTypeTraits<typename UnAlias<T>::type>::name(),
            result);
        }

        template<typename T>
        void constructTypeBefore(T const& v) {

          if (!isEmpty()) {

            jobject jobj = checkPtr(createJavaObject(v));
            if (topItem() != NULL) {

              callParentSetter(
                  topItem(),
                  TypeName<T>::value(),
                  JavaTypeTraits<typename UnAlias<T>::type>::name(),
                  jobj);

              pushItem(jobj);
            } else {

              topItem() = jobj;
            }
          }
        }

        void constructTypeAfter();

        template<typename T>
        void constructPolyTypeBefore(T const &v) {
          if(!isEmpty()) {
            DataType<typename UnAlias<T>::type> const &dt
              = dataType<typename UnAlias<T>::type>();
            std::string const &polyName = dt.dynFromValue(toAny(v)).getName();
            jobject jobj = createJavaObjectAux(
              "L" + getJavaPackage() + polyName + ";", "()V", NULL);
            if(topItem() != NULL) {
              callParentSetter(
                topItem(),
                TypeName<T>::value(),
                JavaTypeTraits<typename UnAlias<T>::type>::name(),
                jobj);
              pushItem(jobj);
            } else
              topItem() = jobj;
          }
        }

        void constructPolyTypeAfter() {
          constructTypeAfter();
        };

        jobject createArray(const std::string& javaTypeName, int len) const;

        jobject createByteArray(int len) const;

        void setByteArrayRegion(jobject jobjArr, int start, int len, const unsigned char *buf) const;

        void pushItem(jobject jobj);

        void popItem();

        jobject& topItem() const;

        bool isEmpty() const;

        int getSize() const;

        void setIndex(int index);

        int getIndex() const;

        void setJniEnv(JNIEnv *je, jclass jc, jobject& jrootObj);

        std::string fieldNameToUse() {return fieldNameToUse_;}
        void fieldNameToUse(const std::string& fieldNameToUse_) {this->fieldNameToUse_ = fieldNameToUse_;}
      private:
        std::string fieldNameToUse_;
        Loki::PimplOf<JniWriterImpl>::Type impl_;
      };

      /*---------------------Dispatchers------------------------------*/
      template<typename T,typename TagT, typename EnableT>
      struct JniWriterDispatcher {
        inline static void
         action(JniWriter<TagT> const& w, T const& v) {
           TRACE_SCOPE(JniWriterImpl, "action", JavaTypeTraits<T>::simpleValue());

           w.impl_.constructTypeBefore(v);
           gmap(unAlias(v),w,Unit());
           w.impl_.constructTypeAfter();

        }
      };

      /*template<typename T,typename TagT>
      struct JniWriterDispatcher<T, TagT,
        typename boost::enable_if <
          boost::is_same<
            typename UnAlias<T>::type,
            qx::crypto::HashType
          >
        >::type
      > {
        inline static typename void
         action(JniWriter<TagT> const& w, T const& v) {
           int a = 1;
        }
       };*/

      template<typename T,typename TagT>
      struct JniWriterDispatcher<T, TagT,
        typename boost::enable_if <
          boost::is_same<
            typename UnAlias<T>::type,
            boost::optional<typename UnAlias<T>::type::value_type>
          >
        >::type
      > {
        inline static void
         action(JniWriter<TagT> const& w, T const& v) {
           TRACE_SCOPE(JniWriterImpl, "action", JavaTypeTraits<T>::simpleValue());

           w.impl_.fieldNameToUse(JavaTypeTraits<T>::simpleValue());

           if (!!unAlias(v)) {
             w(unAlias(v).get());
           }

           w.impl_.fieldNameToUse("");
        }
      };

      template<typename T,typename TagT>
      struct JniWriterDispatcher<T, TagT,
        typename boost::enable_if <
          IsEnum<typename UnAlias<T>::type>
        >::type
      > {
        inline static void
         action(JniWriter<TagT> const& w, T const& v) {
           TRACE_SCOPE(JniWriterImpl, "action", JavaTypeTraits<T>::simpleValue());

           jobject jobj = checkPtr(w.impl_.createJavaObject(static_cast<int>(unAlias(v).val())));

           w.impl_.callParentSetter(
             w.impl_.topItem(),
             TypeName<T>::value(),
             JavaTypeTraits<int>::name(),
             jobj);
        }
      };

      template<typename T,typename TagT>
      struct JniWriterDispatcher<T, TagT,
        typename boost::enable_if <
          IsPrimitive<typename UnAlias<T>::type>
        >::type
      > {
        inline static void
         action(JniWriter<TagT> const& w, T const& v) {
           TRACE_SCOPE(JniWriterImpl, "action", JavaTypeTraits<T>::simpleValue());

           w.impl_.constructPrimitive(v);
        }
      };

      template<typename T,typename TagT>
      struct JniWriterDispatcher<T, TagT,
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
        inline static void
        action(JniWriter<TagT> const& w, T const& v) {
          TRACE_SCOPE(JniWriterImpl, "action", JavaTypeTraits<T>::simpleValue());

          typedef typename UnAlias<T>::type::value_type cont_elem_type_alias;
          typedef typename UnAlias<cont_elem_type_alias>::type cont_elem_type;

          std::string javaTypeName = JavaTypeTraits<cont_elem_type>::name();

          //TODO: handle NULL pointer properly!!
          jobject jobjArr = checkPtr(w.impl_.createArray(javaTypeName, unAlias(v).size()));

          w.impl_.pushItem(jobjArr);

          int i = 0;
          for (typename UnAlias<T>::type::const_iterator it = unAlias(v).begin(); it != unAlias(v).end(); ++it) {
            w.impl_.setIndex(i);
            w(*it);
            ++i;
          }

          w.impl_.popItem();

          if (w.impl_.topItem() != NULL) {

            w.impl_.callParentSetter(
              w.impl_.topItem(),
              TypeName<T>::value(),
              JavaTypeTraits<typename UnAlias<T>::type>::name(),
              jobjArr, true);
          } else {

            w.impl_.topItem() = jobjArr;
          }
        }
      };


      /*template<typename T,typename TagT>
      struct JniWriterDispatcher<T, TagT,

        typename boost::enable_if <
          boost::is_same<
            typename UnAlias<T>::type,
            Binary
          >
        >::type
      > {
        inline static typename void
        action(JniWriter<TagT> const& w, T const& v) {
          std::string base32EncodedValStr(qx::syb::toBase32<std::string>(unAlias(v)));

          jobject jobj = checkPtr(w.impl_.createJavaObject(base32EncodedValStr));

          w.impl_.callParentSetter(
            w.impl_.topItem(),
            "Binary",
            JavaTypeTraits<std::string>::name(),
            jobj);
        }
      };*/


      template<typename T,typename TagT>
      struct JniWriterDispatcher<T, TagT,
        typename boost::enable_if <
          boost::is_same<
            typename UnAlias<T>::type,
            Binary
          >
        >::type
      > {
        inline static void
        action(JniWriter<TagT> const& w, T const& v) {
          TRACE_SCOPE(JniWriterImpl, "action", JavaTypeTraits<T>::simpleValue());

          //std::string base32EncodedValStr(qx::syb::toBase32<std::string>(unAlias(v)));
          if (!unAlias(v).empty()) {
            jobject jByteArr = checkPtr(w.impl_.createByteArray(unAlias(v).size()));
            w.impl_.setByteArrayRegion(jByteArr, 0, unAlias(v).size(), &unAlias(v)[0]);

            if (w.impl_.topItem() != NULL) {

              w.impl_.callParentSetter(
                w.impl_.topItem(),
                "Binary",
                JavaTypeTraits<unsigned char>::typeDigit(),
                jByteArr, true);
            } else {

              w.impl_.topItem() = jByteArr;
            }
          }
        }
      };


      template<typename T,typename TagT>
      struct JniWriterDispatcher<T, TagT,
        typename boost::enable_if <
          boost::is_same<
            typename UnAlias<T>::type,
            std::pair<typename UnAlias<T>::type::first_type, typename UnAlias<T>::type::second_type>
          >
        >::type
      > {
        inline static void
         action(JniWriter<TagT> const& w, T const& v) {
           TRACE_SCOPE(JniWriterImpl, "action", JavaTypeTraits<T>::simpleValue());

          //custom or common?
          //const std::string& javaTypeName = JavaTypeTraits<typename UnAlias<T>::type>::name();
          //const std::string& sign = JavaTypeTraits<typename UnAlias<T>::type>::constrSignature();
          //return createJavaObjectAux(javaTypeName, sign, DUMMY, unAlias(v));

          //jobject jobj = checkPtr(w.impl_.createJavaObject(unAlias(v)));

          w.impl_.constructTypeBefore(v);

          w(unAlias(v).first);
          w(unAlias(v).second);

          w.impl_.constructTypeAfter();

        }
      };

      template<typename T, typename TagT>
      struct JniWriterDispatcher<T, TagT, typename boost::enable_if<mpl::and_<
            NotOpt<typename UnAlias<T>::type>,
            IsMultiConstr<T>
          > >::type> {
        inline static void action(JniWriter<TagT> const &w, T const &v) {
          TRACE_SCOPE(JniWriterImpl, "action", JavaTypeTraits<T>::simpleValue());
          if(v) {
            w.impl_.constructPolyTypeBefore(v);
            gmap(v, w, Unit());
            w.impl_.constructPolyTypeAfter();
          }
        }
      };

      /*template<typename T,typename TagT>
      struct JniWriterDispatcher<T, TagT,
        typename boost::enable_if <
          boost::is_same<
            typename UnAlias<T>::type,
            lcm::TrueFalseBool
          >
        >::type
      > {
        inline static typename void action(JniWriter<TagT> const &w, T const &v) {
           TRACE_SCOPE(JniReaderImpl, "action", JavaTypeTraits<T>::simpleValue());

           w.impl_.fieldNameToUse(JavaTypeTraits<T>::simpleValue());
           w(unAlias(v));
           w.impl_.fieldNameToUse("");
        }
      };*/

      /*template<typename TagT>
      struct JniWriterDispatcher<qx::BinaryData, TagT, void> {
        inline static typename void
        action(JniWriter<TagT> const& w, qx::BinaryData const& v) {

          //TODO: reconsider this one
          //constructTypeBefore===
          jobject jobj = checkPtr(w.impl_.createJavaObjectAux(
            JavaTypeTraits<qx::BinaryData>::name(), JavaTypeTraits<qx::BinaryData>::constrSignature(), JniWriterImpl::DUMMY));

          w.impl_.callParentSetter(
              w.impl_.topItem(),
              TypeName<qx::BinaryData>::value(),
              JavaTypeTraits<qx::BinaryData>::name(),
              jobj);

          w.impl_.pushItem(jobj);
          //===

          if (!unAlias(v).empty()) {
            jobject jByteArr = checkPtr(w.impl_.createByteArray(unAlias(v).size()));

            w.impl_.setByteArrayRegion(jByteArr, 0, unAlias(v).size(), &unAlias(v)[0]);

            w.impl_.callParentSetter(
              w.impl_.topItem(),
              "Bytes",
              JavaTypeTraits<unsigned char>::typeDigit(),
              jByteArr, true);
          }

          w.impl_.constructTypeAfter();
        }
      };*/
    }

    template<typename TagT>
    struct JniWriter:Visitor<JniWriter<TagT> > {
      inline explicit JniWriter(JNIEnv *je, jclass jc, jobject& jrootObj) : jrootObj_(jrootObj) {
        impl_.setJniEnv(je, jc, jrootObj);
      }

      template<typename T>
      inline void operator()(T const& v) const {
        priv::JniWriterDispatcher<T,TagT>::action(*this,v);
        jrootObj_ = impl_.topItem();
      }

    private:
      mutable priv::JniWriterImpl impl_;
      jobject& jrootObj_;

      template<typename T, typename TagU, typename EnableT>
      friend struct priv::JniWriterDispatcher;
    };
  }
}

#endif /*JNI_WRITTER_H*/
