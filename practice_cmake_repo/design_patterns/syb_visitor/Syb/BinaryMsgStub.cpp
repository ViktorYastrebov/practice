
#if 0
#include "Syb/BinaryMsgStub.h"
#include "SybXml2/XmlReader.h"
#include "SybXml2/AnyElementImpl.h"
#include "Syb/cJSON.h"
#else
#include "BinaryMsgStub.h"
#include "../SybXml2/XmlReader.h"
#include "../SybXml2/AnyElementImpl.h"
#include "cJSON.h"
#endif
#include <boost/pointer_cast.hpp>
#include <boost/detail/endian.hpp>

#include <boost/foreach.hpp>

namespace qx {
  namespace msg {
    inline boost::uint32_t reverseBytes(boost::uint32_t value) {
      return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 |
             (value & 0x00FF0000U) >> 8  | (value & 0xFF000000U) >> 24;
    }
    
    inline boost::uint64_t reverseBytes(boost::uint64_t value) {
      return (value & 0x00000000000000FFULL) << 56 | (value & 0x000000000000FF00ULL) << 40 |
             (value & 0x0000000000FF0000ULL) << 24 | (value & 0x00000000FF000000ULL) << 8  |
             (value & 0x000000FF00000000ULL) >> 8  | (value & 0x0000FF0000000000ULL) >> 24 |
             (value & 0x00FF000000000000ULL) >> 40 | (value & 0xFF00000000000000ULL) >> 56;
    }

//Bool
    void doRead(std::istream &input, bool &value) {
      input.get(reinterpret_cast<char &>(value));
    }
    void doWrite(std::ostream &output, bool value) {
      output.put(static_cast<char>(value == true ? 1 : 0));
    }
//Byte
    void doRead(std::istream &input, boost::uint8_t &value) {
      input.get(reinterpret_cast<char &>(value));
    }
    void doWrite(std::ostream &output, boost::uint8_t value) {
      output.put(static_cast<char>(value));
    }
//Float
    void doRead(std::istream &input, float &value) {
      input.read(reinterpret_cast<char *>(&value), sizeof(float));
      #ifdef BOOST_BIG_ENDIAN
        value = reverseBytes(static_cast<boost::uint64_t>(value));
      #endif
    }
    void doWrite(std::ostream &output, float value) {
      #ifdef BOOST_BIG_ENDIAN
        value = reverseBytes(static_cast<boost::uint64_t>(value));
      #endif
      output.write(reinterpret_cast<const char *>(&value), sizeof(float));
    }
//TimeStamp
    void doWrite(std::ostream &output, boost::posix_time::ptime value) {
      static const boost::posix_time::ptime epoch(
        boost::gregorian::date(1970, 1, 1));
      boost::uint64_t x = (value - epoch).total_seconds();
      output.write(reinterpret_cast<char *>(&x), 8);
    }
    void doRead(std::istream &input, boost::posix_time::ptime &value) {
      boost::uint64_t x;
      input.read(reinterpret_cast<char *>(&x), 8);
      value = boost::posix_time::from_time_t(static_cast<time_t>(x));
    }
//UINT32
    //void doWrite(std::ostream &output, const boost::uint32_t &value) {
    //  boost::uint8_t target[5];
    //  target[0] = static_cast<boost::uint8_t>(value | 0x80);
    //  if (value >= (1 << 7)) {
    //    target[1] = static_cast<boost::uint8_t>((value >>  7) | 0x80);
    //    if (value >= (1 << 14)) {
    //      target[2] = static_cast<boost::uint8_t>((value >> 14) | 0x80);
    //      if (value >= (1 << 21)) {
    //        target[3] = static_cast<boost::uint8_t>((value >> 21) | 0x80);
    //        if (value >= (1 << 28)) {
    //          target[4] = static_cast<boost::uint8_t>(value >> 28);
    //          output.write(reinterpret_cast<const char *>(target), 5);
    //        } else {
    //          target[3] &= 0x7F;
    //          output.write(reinterpret_cast<const char *>(target), 4);
    //        }
    //      } else {
    //        target[2] &= 0x7F;
    //        output.write(reinterpret_cast<const char *>(target), 3);
    //      }
    //    } else {
    //      target[1] &= 0x7F;
    //      output.write(reinterpret_cast<const char *>(target), 2);
    //    }
    //  } else {
    //    target[0] &= 0x7F;
    //    output.put(target[0]);
    //  }
    //}
    void doWrite(std::ostream &out, boost::uint32_t value) {
      boost::uint32_t remaining = value >> 7;
    
      while (remaining != 0) {
        out.put((value & 0x7f) | 0x80);
        value = remaining;
        remaining >>= 7;
      }
    
      out.put((value & 0x7f));
    }
    
    void doRead(std::istream &input, boost::uint32_t &value) {
      boost::uint32_t result = 0;
      size_t num_read = 0;
      unsigned int shift = 0;
      boost::uint8_t byte;
      
      do {
        input.get(reinterpret_cast<char &>(byte));
        result |= ((byte & 0x7f)) << shift;
        shift += 7;
      } while (byte & 0x80);
      
      value = result;
    }
//INT32
    boost::uint32_t zigZagEncode32(boost::int32_t n) {
      return (n << 1) ^ (n >> 31);
    }
    boost::int32_t zigZagDecode32(boost::uint32_t n) {
      return (n >> 1) ^ (-static_cast<boost::int32_t>(n & 1));
    }
    void doWrite(std::ostream &output, boost::int32_t value) {
      write(output, zigZagEncode32(value));
    }
    void doRead(std::istream &in, boost::int32_t &value) {
      read(in, reinterpret_cast<boost::uint32_t&>(value));
      value = zigZagDecode32(reinterpret_cast<boost::uint32_t&>(value));
    }
//UINT64
    void doWrite(std::ostream &out, boost::uint64_t value) {
      BOOST_STATIC_ASSERT(sizeof(boost::uint32_t) / sizeof(char) == 4);
      boost::uint64_t remaining = value >> 7;
    
      while (remaining != 0) {
        out.put((value & 0x7f) | 0x80);
        value = remaining;
        remaining >>= 7;
      }
    
      out.put((value & 0x7f));
    }
    void doRead(std::istream &input, boost::uint64_t &value) {
      boost::uint64_t result = 0;
      size_t num_read = 0;
      unsigned int shift = 0;
      boost::uint8_t byte;
      
      do {
        input.get(reinterpret_cast<char &>(byte));
        result |= ((byte & 0x7f)) << shift;
        shift += 7;
      } while (byte & 0x80);
      
      value = result;
    }
//String
    void read(std::istream &input, std::string &value) {
      boost::uint32_t size;
      read(input, size);
      value.resize(size);
      if (size)
        input.read(const_cast<char *>(value.c_str()), size);
    }
    void write(std::ostream &output, const std::string &value) {
      boost::uint32_t size = static_cast<boost::uint32_t>(value.size());
      write(output, size);
      if (size)
        output.write(value.c_str(), size);
    }
//UnicodeString
    void doRead(std::istream &input, UnicodeString &value) {
      std::string str;
      read(input, str);
      value = UnicodeString::fromUTF8(str.c_str());
    }
    void doWrite(std::ostream &output, const UnicodeString &value) {
      std::string result;
      value.toUTF8String(result);
      write(output, result);
    }
    /*
    void doRead(std::istream &input, syb::Any &value) {
      value.setInitializer(boost::make_shared<InitFromBinary>(&input));
    }

    void doWrite(std::ostream &output, const syb::Any &value) {
      std::string res;
      if (value.empty()) {
        if (value.getInitializer()) {
          typedef qx::syb::priv::XmlElementReader<void, syb::Any>::Initializer init_t;
          boost::shared_ptr<init_t> init = boost::dynamic_pointer_cast<init_t>(value.getInitializer());
          assert(init);
          res = init->state()->buf();
        }
        write(output, static_cast<boost::uint8_t>(0));
        write(output, res);      } else {
        value.getDynInfo().getDataType().toBinary(value, res);
        write(output, static_cast<boost::uint8_t>(1));
        write(output, res);
      }
    }
    */
  }
}


namespace qx {
namespace json {

  cJSON *doWrite(bool v, bool typeIdByAttribute) {
    cJSON *node = cJSON_CreateBool(v);
    assert(node);
    return node;
  }

  cJSON *doWrite(int v, bool typeIdByAttribute) {
    cJSON *node = cJSON_CreateNumber(v);
    assert(node);
    return node;
  }

  cJSON *doWrite(unsigned int v, bool typeIdByAttribute) {
    cJSON *node = cJSON_CreateNumber(v);
    assert(node);
    return node;
  }

  cJSON *doWrite(float v, bool typeIdByAttribute) {
    cJSON *node = cJSON_CreateNumber(v);
    assert(node);
    return node;
  }

  cJSON *doWrite(const std::string &v, bool typeIdByAttribute) {
    cJSON *node = cJSON_CreateString(v.c_str());
    assert(node);
    return node;
  }

  cJSON *doWrite(const UnicodeString &v, bool typeIdByAttribute) {
    std::string str;
    v.toUTF8String(str);
    cJSON *node = cJSON_CreateString(str.c_str());
    assert(node);
    return node;
  }

  cJSON *doWrite(const  boost::posix_time::ptime &v, bool typeIdByAttribute) {
    std::string datetime;
    if (!v.is_not_a_date_time()) {
      datetime = boost::posix_time::to_iso_extended_string(v).substr(0, 19);
    }
    cJSON *node = cJSON_CreateString(datetime.c_str());
    assert(node);
    return node;
  }

  cJSON *doWrite(const qx::syb::Any &v, bool typeIdByAttribute) {
    cJSON *node = 0;
    if (v.empty()) {
      if (v.getInitializer()) {
        typedef qx::syb::priv::XmlElementReader<void, syb::Any>::Initializer init_t;
        boost::shared_ptr<init_t> init = boost::dynamic_pointer_cast<init_t>(v.getInitializer());
        assert(init);
        const std::string &xml = init->state()->buf();
        //node = qx::msg::Json::getInstance().convertFromXml(xml);
        node = cJSON_CreateObject();
        cJSON_AddItemToObject(node, "StyleSheet", cJSON_CreateString(xml.c_str()));
      }
    } else {
      node = v.getDynInfo().getDataType().toJson(v);
    }
    if (!node) {
      node = cJSON_CreateObject();
      cJSON_AddItemToObject(node, "_nil", cJSON_CreateTrue());
    }
    return node;
  }
}}
