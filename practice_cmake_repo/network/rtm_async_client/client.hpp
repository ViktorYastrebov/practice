#pragma once

#include "connection.hpp"
#include <thread>

class client {
public:
  client();
  client(const std::string  &host, const std::string  &port);
  ~client();

  client(const client  &cl) = delete;
  client  & operator = (const client  &cl) = delete;

  void set_host(const std::string  &host);
  void set_port(const std::string  &port);

  void connect();
  void send(const std::shared_ptr<base_task>  &task);
private:
  void handle_connect(const boost::system::error_code& error);

private:
  std::string host_;
  std::string port_;
  std::shared_ptr<connection> connection_;
  boost::asio::io_service io_service_;

  std::thread service_thread_;
};