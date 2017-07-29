#pragma once

#include <string>
#include <iosfwd>
#include <thread>

#include "connection_pool.hpp"

class server {
public:
  //server();
  explicit server(const std::string  &ip, unsigned short port);
  ~server();

  void start();

  void set_ip(const std::string  &ip);
  void set_port(unsigned short port);

  //void set_logger(std::shared_ptr<std::ostream>  ostream);

  void assign_to_all(const std::shared_ptr<base_task>  &new_task);
  void assign_to(const std::shared_ptr<base_task>  &new_task, const std::shared_ptr<connection>  &connection);
  void assign_to(const std::shared_ptr<base_task>  &new_task, std::size_t connection_id);

private:
  void accept_handler(const boost::system::error_code& error, std::shared_ptr<connection>  con);
  void accept();
  void exception_handler();
private:
  std::string ip_;
  unsigned short port_;
  boost::asio::io_service  io_service_;
  std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
  connection_pool connection_pool_;
private:
  std::thread service_thread_;
//addtions
//private:
  //std::shared_ptr<std::ostream<char>> logger_;
};