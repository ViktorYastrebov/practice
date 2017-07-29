#pragma once

#include <deque>
#include <boost/asio.hpp>

#include "base_task.h"

class connection : public std::enable_shared_from_this<connection> {
public:
  typedef std::deque<std::shared_ptr<base_task>> task_list_t;

  connection(boost::asio::io_service  &io_service);
  ~connection();

  boost::asio::ip::tcp::socket  & get_socket();

  void send(const std::shared_ptr<base_task>  &task);
  void init_recieving();

private:
  void handle_write_request_length(const boost::system::error_code  &error, std::size_t bytes_transferred);
  void handle_write_request_data(const boost::system::error_code  &error, std::size_t bytes_transferred);

  void handle_read_request_length(const boost::system::error_code  &error, std::size_t bytes_transferred);
  void handle_read_request_data(const boost::system::error_code  &error, std::size_t bytes_transferred);

  void handle_write_response_length(const boost::system::error_code  &error, std::size_t bytes_transferred);
  void handle_write_response_data(const boost::system::error_code  &error, std::size_t bytes_transferred);

  void handle_read_response_length(const boost::system::error_code  &error, std::size_t bytes_transferred);
  void handle_read_response_data(const boost::system::error_code  &error, std::size_t bytes_transferred);

private:
  boost::asio::ip::tcp::socket socket_;
  task_list_t requests_pool_;
  task_list_t responces_pool_;
  task_list_t read_pool_;
};
