#include "expceptions.h"

namespace std {
  //base expception node
  connection_expception::connection_expception(const std::string  &what)
    :std::exception()
  {}
  const char  *connection_expception::what() const throw() {
    return std::exception::what();
  }
  connection_expception::~connection_expception() throw()
  {}


  //signalize just that there is no data
  no_data_exception::no_data_exception(const std::string  &what)
    :connection_expception(what)
  {}
  const char  *no_data_exception::what() const throw() {
    return std::exception::what();
  }
  no_data_exception::~no_data_exception() throw()
  {}

  //signalize that connection has been closed
  connection_closed::connection_closed(const std::string  &what)
    :connection_expception(what)
  {}
  const char  *connection_closed::what() const throw() {
    return std::exception::what();
  }
  connection_closed:: ~connection_closed() throw()
  {}
}
