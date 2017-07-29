#pragma once

#include "connection.hpp"

class SqlStatement {
public:
  virtual ~SqlStatement() = 0
  {}

  virtual int prepare() = 0;
  virtual int execute() = 0;
  virtual int commit() = 0;
  virtual void setSql(const std::string  &query) = 0;

  virtual std::string getErrorMessage() const = 0;
};