#ifndef SYB_XML_TYPE_UNICODE_STRING_H
#define SYB_XML_TYPE_UNICODE_STRING_H
//TODO:!!!!!!!!!!!!!!!!!!!!!!!!!
// includes

#include <boost/format.hpp>
#include <unicode/unistr.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace qx {
  namespace syb {

    inline UnicodeString defaultXmlReaderICUHook(const UnicodeString &v) {
      return v;
    }

    struct XmlReaderICUHook {
      typedef boost::function<UnicodeString(const UnicodeString&)> HookType;
      XmlReaderICUHook() : hook_(boost::bind<UnicodeString>(defaultXmlReaderICUHook, _1)) {
      }

      static XmlReaderICUHook& instance() {
        static XmlReaderICUHook ret;
        return ret;
      }

      void setHook(const HookType &hook) {
        hook_ = hook;
      };

      void resetHook() {
        hook_ = boost::bind<UnicodeString>(defaultXmlReaderICUHook, _1);
      };

      UnicodeString transform(const UnicodeString &v) const {
        return hook_(v);
      }
    private:
      HookType hook_;
    };

    template<>
    struct IsUnicode<std::string,void>:mpl::true_{};
    
    template<>
    struct IsUnicode<UnicodeString,void>:mpl::true_{};
    
    namespace priv {
      template<>
      struct XmlStringToPrim<UnicodeString> {
        inline static void action(UnicodeString& v, UnicodeString const& vs) {
          v = qx::syb::XmlReaderICUHook::instance().transform(vs);
        }
      };

      template<>
      struct XmlStringToPrim<std::string> {
        inline static void action(std::string& v, UnicodeString const& vs) {
          v = "";
          vs.toUTF8String(v);
        }
      };

      template<>
      struct XmlPrimToString<UnicodeString> {
        inline static UnicodeString action(UnicodeString const& v) {
          return v;
        }
      };

      template<>
      struct XmlPrimToString<std::string> {
        inline static UnicodeString action(std::string const& v) {
          if(v.c_str())
            return UnicodeString::fromUTF8(v.c_str());
          return UnicodeString();
        }
      };

      template<typename TagT, typename U>
      struct XmlAttributeReader<TagT,U
              ,typename boost::enable_if<IsUnicode<U> >::type> {
        inline bool read(XmlReader<TagT> const& w, Any const& v) const {
          AbsXmlInfo const& i = v.getDynInfo().getXmlInfo();
          UnicodeString ret 
            = w.impl().getUnicodeAttribute(i.getName());
 /*         if (ret.isEmpty()) {
            if(!i.hasIndependentOrder())
              throw std::exception(
                boost::str(boost::format("\'%1%\'(xmlns:%2%=\'%3%\') was expected but not found") %
                           i.getName().name() % i.getName().nsPrefix() % i.getName().ns()).c_str());
            return false;
          }*/
          XmlStringToPrim<U>::action(fromAny<U>(v),ret);
          return true;
        }
      };

      template<typename TagT, typename U>
      struct XmlAttributeReader<TagT,boost::optional<U> 
              ,typename boost::enable_if<IsUnicode<U> >::type> {
        inline bool read(XmlReader<TagT> const& w, Any const& v) const {
          boost::optional<UnicodeString> ret = w.impl().getOptionalUnicodeAttribute(
            v.getDynInfo().getXmlInfo().getName());
          if (!ret)
            return false;
          boost::optional<U>& vv = fromAny<boost::optional<U> >(v);
          vv.reset(U());
          XmlStringToPrim<U>::action(*vv,ret.get());
          return true;
        }
      };

      template<typename TagT,typename U>
      struct XmlReadContent<TagT,U
        ,typename boost::enable_if<mpl::and_<IsPrimitive<U>, IsUnicode<U> > >::type
      > : AbsXmlReadContent<TagT>
      {
        void read(XmlReader<TagT> const& w, Any const& v) const {
          XmlStringToPrim<U>::action(fromAny<U>(v), w.impl().getUnicodeBody());
        }
      };

      template<typename TagT,typename U>
      struct XmlReadContent<TagT,boost::optional<U>
        ,typename boost::enable_if<mpl::and_<IsPrimitive<U>, IsUnicode<U> > >::type
      > : AbsXmlReadContent<TagT>
      {
        void read(XmlReader<TagT> const& w, Any const& v) const {
          if (!w.impl().getBody().empty()) { //TODO: make more accurate check
            boost::optional<U>& vv = fromAny<boost::optional<U> >(v);
            vv.reset(U());
            XmlStringToPrim<U>::action(*vv, w.impl().getUnicodeBody());
          }
        }
      };

    }

  }
}


#endif
