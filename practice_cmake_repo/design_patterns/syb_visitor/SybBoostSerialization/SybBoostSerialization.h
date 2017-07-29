#ifndef SYB_BOOST_SERIALIZATION_H
#define SYB_BOOST_SERIALIZATION_H

//Some of the Boost::Serialization built-in types support:
#include <boost/serialization/vector.hpp>               //std::vector
#include <boost/serialization/list.hpp>                 //std::list
#include <boost/serialization/string.hpp>               //std::string
#include <boost/serialization/map.hpp>                  //std::map, std::multimap
#include <boost/serialization/set.hpp>                  //std::set, std::multiset
#include <boost/serialization/deque.hpp>                //std::deque
#include <boost/serialization/bitset.hpp>               //std::bitset

#include <boost/serialization/scoped_ptr.hpp>           //boost::scoped_ptr
#include <boost/serialization/shared_ptr.hpp>           //boost::shared_ptr
#include <boost/serialization/weak_ptr.hpp>             //boost::weak_ptr

#include <boost/serialization/variant.hpp>              //boost::variant
#include <boost/serialization/optional.hpp>             //boost::optional

#include <boost/date_time/gregorian/greg_serialize.hpp> //boost::gregorian
//and more...

#include "SybBoostSerialization/UnicodeString.h"        //icu::UnicodeString

#include <boost/serialization/nvp.hpp>

//a portable XML serialization
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

//a portable text serialization
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

//a portable binary serialization
#include "portable_binary_iarchive.hpp"
#include "portable_binary_oarchive.hpp"

#include "Syb/Syb.h"
#include "Syb/Trace.h"

namespace qx {
namespace syb {

  template<typename Archive, typename T, typename EnableT = void>
  struct Bridge;

  template<typename Archive>
  struct SybBoostSerialization:Visitor<SybBoostSerialization<Archive> > {
    inline SybBoostSerialization(Archive& archive) : archive_(archive) {
    }

    template<typename T>
    inline void operator()(T& t) const {
      Bridge<Archive, T>::action(archive_, t);
    }
  private:
    Archive& archive_;
  };

  template<typename Archive, typename T>
  struct Bridge<Archive, T, typename boost::enable_if<EmbeddedInfo<T> >::type> {
    inline static void action(Archive& archive, T& t) {
      TRACE_SCOPE(SybBoostSerialization<Archive>, "go", TypeName<T>::value());
      archive & boost::serialization::make_nvp(T::sybTypeName().c_str(), t);
    }
  };

  template<typename Archive, typename T>
  struct Bridge<Archive, T, typename boost::disable_if<EmbeddedInfo<T> >::type> {
    inline static void action(Archive& archive, T& t) {
      TRACE_SCOPE(SybBoostSerialization<Archive>,
                  (Archive::is_loading::value ? "read" : "write"),
                  TypeName<T>::value());
      archive & boost::serialization::make_nvp("item", t);
    }
  };

//Writers
  template<typename T>
  struct SybBoostXmlWriterManip {
    inline explicit SybBoostXmlWriterManip(T const& t, unsigned int flags):t_(t), flags_(flags) {
    }
    friend inline std::ostream&
    operator<<(std::ostream& stream, SybBoostXmlWriterManip const& writerManip) {
      boost::archive::xml_oarchive archive(stream, writerManip.flags_);
      qx::syb::SybBoostSerialization<boost::archive::xml_oarchive> gen(archive);
      gen(writerManip.t_);
      return stream;
    }
  private:
    T const& t_;
    unsigned int flags_;
  };

  template<typename T>
  struct SybBoostTextWriterManip {
    inline explicit SybBoostTextWriterManip(T const& t, unsigned int flags):t_(t), flags_(flags) {
    }
    friend inline std::ostream&
    operator<<(std::ostream& stream, SybBoostTextWriterManip const& writerManip) {
      boost::archive::text_oarchive archive(stream, writerManip.flags_);
      qx::syb::SybBoostSerialization<boost::archive::text_oarchive> gen(archive);
      gen(writerManip.t_);
      return stream;
    }
  private:
    T const& t_;
    unsigned int flags_;
  };

  template<typename T>
  struct SybBoostBinaryWriterManip {
    inline explicit SybBoostBinaryWriterManip(T const& t, unsigned int flags):t_(t), flags_(flags) {
    }
    friend inline std::ostream&
    operator<<(std::ostream& stream, SybBoostBinaryWriterManip const& writerManip) {
      portable_binary_oarchive archive(stream, writerManip.flags_);
      qx::syb::SybBoostSerialization<portable_binary_oarchive> gen(archive);
      gen(writerManip.t_);
      return stream;
    }
  private:
    T const& t_;
    unsigned int flags_;
  };

  template<typename T>
  SybBoostXmlWriterManip<T> toBoostXml(T const& t, unsigned int flags = 0) {
    return SybBoostXmlWriterManip<T>(t, flags);
  }

  template<typename T>
  SybBoostTextWriterManip<T> toBoostText(T const& t, unsigned int flags = 0) {
    return SybBoostTextWriterManip<T>(t, flags);
  }

  template<typename T>
  SybBoostBinaryWriterManip<T> toBoostBinary(T const& t, unsigned int flags = boost::archive::no_header) {
    return SybBoostBinaryWriterManip<T>(t, flags);
  }

//Readers
  template<typename T>
  struct SybBoostXmlReaderManip {
    inline explicit SybBoostXmlReaderManip(T& t, unsigned int flags):t_(t), flags_(flags) {
    }
    friend inline std::istream&
    operator>>(std::istream& stream, SybBoostXmlReaderManip const& readerManip) {
      boost::archive::xml_iarchive archive(stream, readerManip.flags_);
      qx::syb::SybBoostSerialization<boost::archive::xml_iarchive> gen(archive);
      gen(readerManip.t_);
      return stream;
    }
  private:
    T& t_;
    unsigned int flags_;
  };

  template<typename T>
  struct SybBoostTextReaderManip {
    inline explicit SybBoostTextReaderManip(T& t, unsigned int flags):t_(t), flags_(flags) {
    }
    friend inline std::istream&
    operator>>(std::istream& stream, SybBoostTextReaderManip const& readerManip) {
      boost::archive::text_iarchive archive(stream);
      qx::syb::SybBoostSerialization<boost::archive::text_iarchive> gen(archive);
      gen(readerManip.t_);
      return stream;
    }
  private:
    T& t_;
    unsigned int flags_;
  };

  template<typename T>
  struct SybBoostBinaryReaderManip {
    inline explicit SybBoostBinaryReaderManip(T& t, unsigned int flags):t_(t), flags_(flags) {
    }
    friend inline std::istream&
    operator>>(std::istream& stream, SybBoostBinaryReaderManip const& readerManip) {
      portable_binary_iarchive archive(stream, readerManip.flags_);
      qx::syb::SybBoostSerialization<portable_binary_iarchive> gen(archive);
      gen(readerManip.t_);
      return stream;
    }
  private:
    T& t_;
    unsigned int flags_;
  };

  template<typename T>
  SybBoostXmlReaderManip<T> fromBoostXml(T& t, unsigned int flags = 0) {
    return SybBoostXmlReaderManip<T>(t, flags);
  }
  template<typename T>
  SybBoostTextReaderManip<T> fromBoostText(T& t, unsigned int flags = 0) {
    return SybBoostTextReaderManip<T>(t, flags);
  }
  template<typename T>
  SybBoostBinaryReaderManip<T> fromBoostBinary(T& t, unsigned int flags = 0) {
    return SybBoostBinaryReaderManip<T>(t, flags);
  }
}  // syb
}  // qx

template<typename Archive>
struct TraceName<qx::syb::SybBoostSerialization<Archive> > {
  inline static std::string const& value() {
    static std::string ret = "bsrl:";
    return ret;
  }
};

namespace boost {
namespace serialization {

  template<typename T>
  typename boost::enable_if<qx::syb::EmbeddedInfo<T> >::type
  inline serialize(boost::archive::xml_oarchive& archive, T& t,
                   const unsigned int) {
    TRACE_SCOPE(qx::syb::SybBoostSerialization<boost::archive::xml_oarchive>,
                "go", qx::syb::TypeName<T>::value());
    gmap(t, qx::syb::SybBoostSerialization<boost::archive::xml_oarchive>(archive),
         qx::syb::Unit());
  }

  template<typename T>
  typename boost::enable_if<qx::syb::EmbeddedInfo<T> >::type
  inline serialize(boost::archive::xml_iarchive& archive, T& t,
                   const unsigned int) {
    TRACE_SCOPE(qx::syb::SybBoostSerialization<boost::archive::xml_iarchive>,
                "go", qx::syb::TypeName<T>::value());
    gmap(t, qx::syb::SybBoostSerialization<boost::archive::xml_iarchive>(archive),
         qx::syb::Unit());
  }

  template<typename T>
  typename boost::enable_if<qx::syb::EmbeddedInfo<T> >::type
  inline serialize(boost::archive::text_oarchive& archive, T& t,
                   const unsigned int) {
    TRACE_SCOPE(qx::syb::SybBoostSerialization<boost::archive::text_oarchive>,
                "go", qx::syb::TypeName<T>::value());
    gmap(t, qx::syb::SybBoostSerialization<boost::archive::text_oarchive>(archive),
         qx::syb::Unit());
  }

  template<typename T>
  typename boost::enable_if<qx::syb::EmbeddedInfo<T> >::type
  inline serialize(boost::archive::text_iarchive& archive, T& t,
                   const unsigned int) {
    TRACE_SCOPE(qx::syb::SybBoostSerialization<boost::archive::text_iarchive>,
                "go", qx::syb::TypeName<T>::value());
    gmap(t, qx::syb::SybBoostSerialization<boost::archive::text_iarchive>(archive),
         qx::syb::Unit());
  }

  template<typename T>
  typename boost::enable_if<qx::syb::EmbeddedInfo<T> >::type
  inline serialize(portable_binary_oarchive& archive, T& t,
                   const unsigned int) {
    TRACE_SCOPE(qx::syb::SybBoostSerialization<portable_binary_oarchive>,
                "go", qx::syb::TypeName<T>::value());
    gmap(t, qx::syb::SybBoostSerialization<portable_binary_oarchive>(archive),
         qx::syb::Unit());
  }

  template<typename T>
  typename boost::enable_if<qx::syb::EmbeddedInfo<T> >::type
  inline serialize(portable_binary_iarchive& archive, T& t,
                   const unsigned int) {
    TRACE_SCOPE(qx::syb::SybBoostSerialization<portable_binary_iarchive>,
                "go", qx::syb::TypeName<T>::value());
    gmap(t, qx::syb::SybBoostSerialization<portable_binary_iarchive>(archive),
         qx::syb::Unit());
  }

}  // serialization
}  // boost

#endif  // SYB_BOOST_SERIALIZATION_H
