#pragma once

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

class session_async
  : public std::enable_shared_from_this<session_async> {
public:
  session_async(boost::asio::ip::tcp::socket socket);
  void start();
private:
  void do_read();
  void do_write(std::size_t length);

  boost::asio::ip::tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

class server
{
public:
  server(boost::asio::io_service& io_service, const std::string  &ip, unsigned short port);
private:
  void do_accept();
  boost::asio::ip::tcp::acceptor acceptor_;
  boost::asio::ip::tcp::socket socket_;
};

void start_async_server();
