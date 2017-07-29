#include "tcp_server_multi_async.hpp"

/*
connection::connection(boost::asio::io_service  &io_service)
  :socket_(io_service)
{}

connection::~connection()
{}*/

void connection::stop() {
  socket_.close();
}

boost::asio::ip::tcp::socket  & connection::socket() {
  return socket_;
}
/*
void connection::handle_write_data(const boost::system::error_code  &error, std::size_t bytes_transferred) {
  if (!error) {
    throw std::exception("data has not been sent");
  }
}*/
