#include <thread>
#include <algorithm>

#include "server.hpp"

#include <iostream>

server::server(const std::string  &ip, unsigned short port)
  :ip_(ip), port_(port), io_service_() 
{
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(ip_), port_);
  acceptor_ = std::make_shared<boost::asio::ip::tcp::acceptor>(io_service_, endpoint);
}

server::~server()
{
  io_service_.stop();
  service_thread_.join();
}

void server::start() {
  accept();
  service_thread_ = std::thread([this]()
  {
    try {
      io_service_.run();
    } catch (std::exception  &ex) {
      //std::cout << "exception occurs: " << ex.what() << std::endl;
    }
  });
//  service_thread_.join();
}

void server::accept() {
  std::shared_ptr<connection> new_connection = std::make_shared<connection>(acceptor_->get_io_service());
  acceptor_->async_accept(new_connection->get_socket(), std::bind(&server::accept_handler, this, std::placeholders::_1, new_connection));
}


void server::accept_handler(const boost::system::error_code& error, std::shared_ptr<connection>  con) {
  if (!error) {
    connection_pool_.get_connections().push_back(con);
    std::cout << "connected ..." << std::endl;

    //it's not quite place for this!!!
    //con->init_recieving(); --breaks

    /*if (logger_) {
      (*logger_) << "connected" << std::endl;
    }*/
  }
  accept();
}

void server::set_ip(const std::string  &ip) {

}

void server::set_port(unsigned short port) {

}

void server::assign_to_all(const std::shared_ptr<base_task>  &new_task) {
  connection_pool::connections_t  &cons = connection_pool_.get_connections();
  if (!cons.empty()) {
    std::for_each(cons.begin(), cons.end(), [&](std::shared_ptr<connection>  &con)
    {
      //con->add(new_task);
      //con->execute_last();
      //con->execute_directly(new_task);
      con->send(new_task);
    });
  }
}

void server::assign_to(const std::shared_ptr<base_task>  &new_task, const std::shared_ptr<connection>  &connection) {

}

void server::assign_to(const std::shared_ptr<base_task>  &new_task, std::size_t connection_id) {

}

void server::exception_handler() {

}
/*
void server::set_logger(std::shared_ptr<std::ostream>  ostream) {
  logger_ = ostream;
}*/