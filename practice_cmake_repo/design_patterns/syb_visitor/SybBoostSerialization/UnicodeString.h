#ifndef SYB_BOOST_SERIALIZATION_UNICODE_STRING_H
#define SYB_BOOST_SERIALIZATION_UNICODE_STRING_H

#include <boost/serialization/serialization.hpp>
#include <unicode/unistr.h>

//!icu::UnicodeString support to boost::serialization.
namespace boost {
namespace serialization {

  template<class Archive>
  inline void save(Archive& archive, const icu::UnicodeString& v, const unsigned int) {
    std::string str;
    str = v.toUTF8String(str);
    archive << boost::serialization::make_nvp("item", str);
  }
  template<class Archive>
  inline void load(Archive& archive, icu::UnicodeString& v, const unsigned int) {
    std::string str;
    archive >> boost::serialization::make_nvp("item", str);
    v = UnicodeString::fromUTF8(str);
  }
  template<class Archive>
  inline void serialize(Archive& ar, icu::UnicodeString& v, const unsigned int fv) {
    split_free(ar, v, fv);
  }

}  // serialization
}  // boost

#endif  // SYB_BOOST_SERIALIZATION_UNICODE_STRING_H
