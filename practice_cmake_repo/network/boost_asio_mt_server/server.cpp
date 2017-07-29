#include "server.hpp"


server::server(unsigned short port, const std::string  &ip)
  : port_(port), ip_(ip)
{
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(ip_), port_);
  acceptor_ = std::make_shared<boost::asio::ip::tcp::acceptor>(io_service_, endpoint);
}
server::~server()
{}

void server::start() {

}

void server::stop() {

}