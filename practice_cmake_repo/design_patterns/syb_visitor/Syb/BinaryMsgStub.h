#ifndef SYB_BINARY_MSG_STUB_H
#define SYB_BINARY_MSG_STUB_H

#include <stdexcept>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_fundamental.hpp>
#include <boost/detail/endian.hpp>
#include <boost/cstdint.hpp>
#include <boost/optional.hpp>
#include <list>

//#include "Syb/Any.h"
#include "Any.h"

struct cJSON;

static const boost::uint8_t UPDATE     = 0;
static const boost::uint8_t RECORD     = 1;
static const boost::uint8_t ARRAY      = 2;
static const boost::uint8_t HIST_ARRAY = 3;


namespace qx {
  namespace msg {

    template<typename T>
    inline void write(std::ostream &output, const T& v) {
      doWrite(output, v);
    }
    template<typename T>
    inline void read(std::istream &input, T& v) {
      doRead(input, v);
    }

    template<typename T>
    inline void doWrite(std::ostream &output, const T& v) {
      throw std::runtime_error("");
    }
    template<typename T>
    inline void doRead(std::istream &input, T& v) {
      throw std::runtime_error("");
    }

  //Bool
  void doRead(std::istream &input, bool &value);
  void doWrite(std::ostream &output, bool value);
  //Byte
    void doRead(std::istream &input, boost::uint8_t &value);
    void doWrite(std::ostream &output, boost::uint8_t value);
  //Float
    void doRead(std::istream &input, float &value);
    void doWrite(std::ostream &output, float value);
  //TimeStamp
    void doWrite(std::ostream &output, boost::posix_time::ptime value);
    void doRead(std::istream &input, boost::posix_time::ptime &value);
  //UINT32
    void doWrite(std::ostream &out, boost::uint32_t value);
    void doRead(std::istream &input, boost::uint32_t &value);
  //INT32
    void doWrite(std::ostream &output, boost::int32_t value);
    void doRead(std::istream &in, boost::int32_t &value);
  //UINT64
    void doWrite(std::ostream &out, boost::uint64_t value);
    void doRead(std::istream &input, boost::uint64_t &value);
  //String
    void read(std::istream &input, std::string &value);
    void write(std::ostream &output, const std::string &value);
  //UnicodeString
    void doRead(std::istream &input, UnicodeString &value);
    void doWrite(std::ostream &output, const UnicodeString &value);

    //void doWrite(std::ostream &output, const qx::syb::Any &value);
    //void doRead(std::istream &input, syb::Any &value);
  }
}


namespace qx {
  namespace json {

    template<typename T>
    inline cJSON *write(T const& v, bool typeIdByAttribute = false) {
      return doWrite(v, typeIdByAttribute);
    }

    template<typename T>
    inline cJSON *doWrite(T const&, bool typeIdByAttribute = false) {
      throw std::runtime_error("");
    }

    cJSON *doWrite(bool v, bool typeIdByAttribute = false);
    cJSON *doWrite(int v, bool typeIdByAttribute = false);
    cJSON *doWrite(unsigned int v, bool typeIdByAttribute = false);

    cJSON *doWrite(float v, bool typeIdByAttribute = false);
    cJSON *doWrite(const std::string &v, bool typeIdByAttribute = false);
    cJSON *doWrite(const UnicodeString &v, bool typeIdByAttribute = false);
    cJSON *doWrite(const boost::posix_time::ptime &v, bool typeIdByAttribute = false);
    cJSON *doWrite(const qx::syb::Any &v, bool typeIdByAttribute = false);
  }
}
#endif
