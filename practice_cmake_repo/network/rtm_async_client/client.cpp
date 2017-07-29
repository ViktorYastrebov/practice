#include <memory>
#include <boost/asio/ip/tcp.hpp>

#include <iostream>
#include "client.hpp"

client::client()
{}

client::client(const std::string  &host, const std::string  &port)
  :host_(host), port_(port), io_service_()
{
  connection_ = std::make_shared<connection>(io_service_);
  //not quite good place for it!!!
  //connection_->init_recieving();
}

client::~client() {
  io_service_.stop();
  service_thread_.join();
}

void client::set_host(const std::string  &host) {
  host_ = host;
}

void client::set_port(const std::string  &port) {
  port_ = port;
}

void client::connect() {
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::resolver::iterator endpoint_iter = resolver.resolve({ host_, port_ });
  boost::asio::async_connect(connection_->get_socket(), endpoint_iter, std::bind(&client::handle_connect, this, std::placeholders::_1));

  //not quite good place for it!!!
  connection_->init_recieving();

  service_thread_ = std::thread([this]() 
    {
      try {
        io_service_.run();
      }
      catch (std::exception  &ex)
      {
        std::cout << ex.what() << std::endl;
      }
    }
  );
}

void client::send(const std::shared_ptr<base_task>  &task) {
  connection_->send(task);
}

void client::handle_connect(const boost::system::error_code& error) {
                            //const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it
  if (!error) {
    std::cout << "connected ..." << std::endl;
  }
}