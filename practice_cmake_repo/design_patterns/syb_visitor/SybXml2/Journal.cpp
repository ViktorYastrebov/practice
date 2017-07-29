#include <fstream>
#include "Journal.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;

namespace qx {
  namespace syb {

    Journal& Journal::instance() {
      static Journal ret = Journal();
      return ret;
    }

    std::string Journal::getPostdate() {
      return std::string();
    }

    std::ostream& JournalOpts::out() {
      static std::ofstream ret("server_journal.xml", std::ios_base::out);
      return ret;
    }

    std::string getCurrentTime() {
      ptime t(microsec_clock::universal_time());

      return to_simple_string(t);
    }
  }
}
