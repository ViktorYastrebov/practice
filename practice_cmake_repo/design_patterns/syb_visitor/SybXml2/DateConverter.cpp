#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/time_parsing.hpp>
#include <boost/format.hpp>
#include "DateConverter.h"

namespace qx {
  namespace syb {

    // Convert date type to string in format "YYYY-MM-DD"
    std::string DateConverter::dateToString(boost::gregorian::date const& d) {
      if (d.is_not_a_date())
        return std::string();

      return boost::gregorian::to_iso_extended_string(d);
    }

    // Convert string in format "YYYY-MM-DD" to date type
    boost::gregorian::date DateConverter::stringToDate(std::string const& str) {
      if (!str.compare("not-a-date-time") || str.empty())
         return boost::gregorian::date();

      return boost::gregorian::from_string(str.substr(0,10));
    }

    // Convert datetime type to string in format "YYYY-MM-DDTHH:MM:SS"
    std::string DateConverter::datetimeToString(boost::posix_time::ptime const& dt) {
      if (dt.is_not_a_date_time())
        return std::string();

      return boost::posix_time::to_iso_extended_string(dt).substr(0, 19);
    }

    // Convert string in format "YYYY-MM-DDTHH:MM:SS" to datetime type
    boost::posix_time::ptime DateConverter::stringToDatetime(std::string const& str) {
      if (!str.compare("not-a-date-time") || str.empty())
        return boost::posix_time::ptime();

      return boost::date_time::parse_delimited_time<boost::posix_time::ptime>(str, 'T');
    }

  }
}
