#pragma once

#include <exception>
#include <string>

#ifndef _WINNT
#define GLIBC_EXPCEPTION _GLIBCXX_USE_NOEXCEPT
#else
#define GLIBC_EXPCEPTION
#endif

namespace std {
  class connection_expception : public exception
  {
  public:
    connection_expception(const std::string  &what);
    virtual const char  *what() const throw();
    virtual ~connection_expception() throw();
  };
  class no_data_exception : public connection_expception
  {
  public:
    no_data_exception(const std::string  &what);
    virtual const char  *what() const throw(); //GLIBC_EXPCEPTION;
    virtual ~no_data_exception() throw(); //GLIBC_EXPCEPTION;
  };

  class connection_closed : public connection_expception
  {
  public:
    connection_closed(const std::string  &what);
    virtual const char  *what() const throw(); //GLIBC_EXPCEPTION;
    virtual ~connection_closed() throw();
  };
}
