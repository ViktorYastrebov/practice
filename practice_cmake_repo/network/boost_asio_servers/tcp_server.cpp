#include "tcp_server.h"
#include <cinttypes>

using boost::asio::ip::tcp;

Server&  Server::Instance(const std::string  &ip_address, unsigned short port) {
  static Server instance(ip_address, port);
  return instance;
}

Server::Server(const std::string  &ip_address, unsigned short port)
  : ip_address_(ip_address), port_(port)
{}

Server::~Server()
{
  stop();
}

void Server::run() {
  acceptor_ = std::make_shared<tcp::acceptor>(io_service_, tcp::endpoint(boost::asio::ip::address_v4::from_string(ip_address_), port_));
  socket_ = std::make_shared<tcp::socket>(io_service_);
  acceptor_->accept(*socket_.get());
}

void Server::stop() {
  acceptor_->close();
  socket_->close();
}

const std::string  & Server::getIpAddress() const {
  return ip_address_;
}

const unsigned short  &Server::getPort() const {
  return port_;
}

void Server::setIpAddress(const std::string  &ip_address) {
  ip_address_ = ip_address;
}

void Server::setPort(unsigned short port) {
  port_ = port;
}

void Server::write_length(const std::uint32_t  &value) {
  static const int bufferLength = 4;
  char buffer[bufferLength];
  std::uint32_t len = htonl(value);

  buffer[0] = len >> 0;
  buffer[1] = len >> 8;
  buffer[2] = len >> 16;
  buffer[3] = len >> 24;

  boost::system::error_code error;
  size_t written_len = boost::asio::write(*socket_.get(), boost::asio::buffer(buffer, bufferLength), error);
}

//template class Server::write<int>(const int &value);
//template class void Server::write<unsigned int>(const unsigned int &value);