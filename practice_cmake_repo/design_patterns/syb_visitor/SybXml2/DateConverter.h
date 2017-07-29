#ifndef SYB_XML_DATE_CONVERTER_H
#define SYB_XML_DATE_CONVERTER_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace qx {
  namespace syb {

    struct DateConverter {

      // Convert date type to string in format "YYYY-MM-DD"
      static std::string dateToString(boost::gregorian::date const& d);

      // Convert string in format "YYYY-MM-DD" to date type
      static boost::gregorian::date stringToDate(std::string const& str);

      // Convert datetime type to string in format "YYYY-MM-DDTHH:MM:SS"
      static std::string datetimeToString(boost::posix_time::ptime const& dt);

      // Convert string in format "YYYY-MM-DDTHH:MM:SS" to datetime type
      static boost::posix_time::ptime stringToDatetime(std::string const& str);

    };

  }
}

#endif
