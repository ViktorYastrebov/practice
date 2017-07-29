#pragma once

#include "connection.hpp"

#include <list>
#include <initializer_list>


class connection_pool {
public:
  typedef std::list<std::shared_ptr<connection>> connections_t;

  connection_pool();
  connection_pool(const std::initializer_list<std::shared_ptr<connection>> &list);
  ~connection_pool();

  std::list<std::shared_ptr<connection>>  &get_connections();
  const std::list<std::shared_ptr<connection>>  &get_connections() const;
private:
  connections_t connections_;
};
