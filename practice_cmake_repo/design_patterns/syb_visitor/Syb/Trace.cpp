#include "Trace.h"
#include <iostream>
#include <boost/iostreams/device/null.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/categories.hpp>

//#include "QxConfigData.h"

#include <memory>

namespace boost {
namespace iostreams {

  class device_sink {
  public:
    typedef char      char_type;
    typedef sink_tag  category;

    device_sink(std::string const &deviceName) : deviceName_(deviceName) {
    }
    std::streamsize write(const char* s, std::streamsize n) {
      //QxConfigData::Instance().log(xioDeviceCode_, std::string(s, n));
      return n;
    }

  private:
    std::string deviceName_;
  };

}  // iostreams
}  // boost



namespace qx {
namespace syb {

  typedef boost::iostreams::stream_buffer<boost::iostreams::null_sink> null_sink_buff_t;

  inline boost::iostreams::null_sink GetNullSink()
  {
    return boost::iostreams::null_sink();
  }

  std::ostream& nullOutput() {

    
    static null_sink_buff_t buff(GetNullSink());
    static std::ostream ret(&buff);
    return ret;
  }
  std::ostream& stdcerrOutput() {
    return std::cerr;
  }
  std::ostream& fileOutput(std::string const &param) {
    static boost::iostreams::stream_buffer<boost::iostreams::file_sink> buf(param.c_str());
    static std::ostream ret(&buf);
    return ret;
  }
  std::ostream& lyciaOutput(std::string const &param) {
    static boost::iostreams::stream_buffer<boost::iostreams::device_sink> buf(param.c_str());
    static std::ostream ret(&buf);
    return ret;
  }

}  // syb
}  // qx
