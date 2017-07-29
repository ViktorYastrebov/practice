#include "async_tcp_echo_server.h"

using boost::asio::ip::tcp;

session_async::session_async(tcp::socket socket)
  : socket_(std::move(socket))
{}

void session_async::start()
{
  do_read();
}

void session_async::do_read()
{
  auto self(shared_from_this());
  socket_.async_read_some(boost::asio::buffer(data_, max_length),
    [this, self](boost::system::error_code ec, std::size_t length)
  {
    if (!ec)
    {
      do_write(length);
    }
  });
}

void session_async::do_write(std::size_t length)
{
  auto self(shared_from_this());
  boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
    [this, self](boost::system::error_code ec, std::size_t /*length*/)
  {
    if (!ec)
    {
      do_read();
    }
  });
}

server::server(boost::asio::io_service& io_service, const std::string  &ip, unsigned short port)
  : acceptor_(io_service, tcp::endpoint(boost::asio::ip::address_v4::from_string(ip), port)), socket_(io_service)
{
  do_accept();
}

void server::do_accept()
{
  acceptor_.async_accept(socket_,
    [this](boost::system::error_code ec)
  {
    if (!ec)
    {
      std::make_shared<session_async>(std::move(socket_))->start();
    }

    do_accept();
  });
}

void start_async_server() {
  try
  {
    std::string ip("127.0.0.1");
    unsigned short port = 2737;

    boost::asio::io_service io_service;
    server s(io_service, ip, port);

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
