#include "connection_pool.hpp"

connection_pool::connection_pool()
{}

connection_pool::connection_pool(const std::initializer_list<std::shared_ptr<connection>> &list)
  :connections_(list)
{}

connection_pool::~connection_pool()
{}

std::list<std::shared_ptr<connection>>  &connection_pool::get_connections()
{
  return connections_;
}

const std::list<std::shared_ptr<connection>>  &connection_pool::get_connections() const
{
  return connections_;
}