#ifndef SYB_XML_TYPE_DATETIME_H
#define SYB_XML_TYPE_DATETIME_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "DateConverter.h"

//#include "SybXml2/XmlReader.h"
#include "XmlReader.h"

namespace qx {
  namespace syb {

    namespace priv {

      // -------------- DateTime --------------
      template<>
      struct XmlStringToPrim<boost::posix_time::ptime> {
        inline static void action(boost::posix_time::ptime& v, std::string const& vs) {
          v = DateConverter::stringToDatetime(vs);
        }
      };

      template<>
      struct XmlPrimToString<boost::posix_time::ptime> {
        inline static std::string action(boost::posix_time::ptime const& v) {
          return DateConverter::datetimeToString(v);
        }
      };



      // ------------ Date -------------
      template<>
      struct XmlStringToPrim<boost::gregorian::date> {
        inline static void action(boost::gregorian::date& v, std::string const& vs) {
          v = DateConverter::stringToDate(vs);
        }
      };

      template<>
      struct XmlPrimToString<boost::gregorian::date> {
        inline static std::string action(boost::gregorian::date const& v) {
          return DateConverter::dateToString(v);
        }
      };

    }

    template<>
    struct StringRepr<boost::posix_time::ptime> : mpl::true_ {
      static bool fromString(boost::posix_time::ptime& v, UnicodeString const& s) {
        std::string us;
        us = s.toUTF8String(us);
        v = DateConverter::stringToDatetime(us);
        return true;
      }
      static void toString(boost::posix_time::ptime const& v, UnicodeString& s) {
        s = UnicodeString::fromUTF8(DateConverter::datetimeToString(v));
      }
    };

    template<>
    struct StringRepr<boost::optional<boost::posix_time::ptime> > : mpl::true_ {
      static bool fromString(boost::optional<boost::posix_time::ptime>& v, UnicodeString const& s) {
        if (s.isEmpty()) 
          return true;
        std::string us;
        us = s.toUTF8String(us);
        v = DateConverter::stringToDatetime(us);
        return true;
      }
      static void toString(boost::optional<boost::posix_time::ptime> const& v, UnicodeString& s) {
        if (v)
          return;
        s = UnicodeString::fromUTF8(DateConverter::datetimeToString(*v));
      }
    };

    template<>
    struct StringRepr<boost::gregorian::date> : mpl::true_ {
      static bool fromString(boost::gregorian::date& v, UnicodeString const& s) {
        std::string us;
        us = s.toUTF8String(us);
        v = DateConverter::stringToDate(us);
        return true;
      }
      static void toString(boost::gregorian::date const& v, UnicodeString& s) {
        s = UnicodeString::fromUTF8(DateConverter::dateToString(v));
      }
    };

    template<>
    struct StringRepr<boost::optional<boost::gregorian::date> > : mpl::true_ {
      static bool fromString(boost::optional<boost::gregorian::date>& v, UnicodeString const& s) {
        if (s.isEmpty()) 
          return true;
        std::string us;
        us = s.toUTF8String(us);
        v = DateConverter::stringToDate(us);
        return true;
      }
      static void toString(boost::optional<boost::gregorian::date> const& v, UnicodeString& s) {
        if (v)
          return;
        s = UnicodeString::fromUTF8(DateConverter::dateToString(*v));
      }
    };

  }
}

#endif
