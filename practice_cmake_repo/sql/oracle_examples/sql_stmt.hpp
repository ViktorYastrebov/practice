#pragma once

#include <memory>
#include "connection.h"

class SqlStatement {
public:
  virtual ~SqlStatement() = 0
  {}

  virtual sword prepare() = 0;
  virtual sword execute() = 0;
  virtual sword commit() = 0;
  virtual void setSql(const std::string  &query) = 0;

  //added for transactions
  virtual std::shared_ptr<Connection> getConnection() = 0;
  virtual const std::shared_ptr<Connection> getConnection() const = 0;
};