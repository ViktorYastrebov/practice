#include "blooking_echo_server.h"

void session(tcp::socket sock)
{
  try
  {
    for (;;)
    {
      char data[max_length];

      boost::system::error_code error;
      size_t length = sock.read_some(boost::asio::buffer(data), error);
      if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw boost::system::system_error(error); // Some other error.

      boost::asio::write(sock, boost::asio::buffer(data, length));
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception in thread: " << e.what() << "\n";
  }
}

void server(boost::asio::io_service& io_service, const std::string  &ip, unsigned short port)
{
  tcp::acceptor a(io_service, tcp::endpoint(boost::asio::ip::address_v4::from_string(ip), port));
  for (;;)
  {
    tcp::socket sock(io_service);
    a.accept(sock);
    std::thread(session, std::move(sock)).detach();
  }
}

void start_blocking_server() {
  try
  {
    std::string ip("127.0.0.1");
    unsigned short port_number = 2738;
    boost::asio::io_service io_service;
    server(io_service, ip, port_number);
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

}