#include "tcp_client.h"

using boost::asio::ip::tcp;

Client::Client(const std::string  &host, const std::string  &port)
  :host_(host), port_(port)
{}

Client::~Client()
{}

void Client::connect() {

  tcp::resolver resolver(io_service_);
  tcp::resolver::query query(host_, port_);
  tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
  socket_ = std::make_shared<tcp::socket>(io_service_);

  boost::system::error_code error;
  boost::asio::connect(*socket_.get(), endpoint_iterator, error);
  if (error) {
    throw std::connection_expception(error.message());
  }
}

void Client::connect(const std::string  &host, const std::string  &port) {
  host_ = host;
  port_ = port;
  connect();
}

void Client::disconnect() {
  socket_->close();
  socket_.reset();
}

std::uint32_t Client::read_length() {
  boost::system::error_code error;
  std::uint32_t net_length = 0;
  socket_->read_some(boost::asio::buffer(reinterpret_cast<char*>(&net_length), sizeof(net_length)), error);
  if (error == boost::asio::error::eof) {
    throw std::connection_closed("connection has been closed by peer");
  } else if (error) {
    throw boost::system::system_error(error); // Some other error.
  }
  return ntohl(net_length);
}