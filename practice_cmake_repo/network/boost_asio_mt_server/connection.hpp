#pragma once

#include <boost/asio.hpp>
#include <future>


template<SocketType>
class connection {
public:
  connection(SocketType &&socket);
  connection(SocketType &&socket, std::packaged_task<void()>  && task);
  ~connection();

  void execute();

private:
  void handle_read_size(const boost::system::error_code  &error, std::size_t bytes_transferred);
  void handle_read_data(const boost::system::error_code  &error, std::size_t bytes_transferred);

  void handle_write_size(const boost::system::error_code  &error, std::size_t bytes_transferred);
  void handle_write_data(const boost::system::error_code  &error, std::size_t bytes_transferred);

private:
  SocketType socket_;
  std::packaged_task<void()> task_;
};
