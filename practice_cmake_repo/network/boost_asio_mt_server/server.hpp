#pragma once

#include <string>
#include "connection_pool.hpp"

class server {
public:
  explicit server(unsigned short port, const std::string  &ip);
  ~server();

  void start();
  void stop();

private:
  void accept();
  void accept_handler(const boost::system::error_code& error, std::shared_ptr<connection>  &&con);
private:
  unsigned short port_;
  std::string ip_;

  std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
  boost::asio::io_service  io_service_;
  connection_pool connection_pool_;
};