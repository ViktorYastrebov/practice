
//http://stackoverflow.com/questions/15434753/boost-asio-multiple-client-connections-to-different-servers
/*
int main(int argc, char*argv[]) {
  
  return 0;
}*/



#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include <thread>
#include <chrono>

#include "tcp_server.h"
#include "blooking_echo_server.h"
#include "async_tcp_echo_server.h"

#include "tcp_server_multi_async.hpp"


using  boost::asio::ip::tcp;
//using namespace std::chrono_literals;
using namespace std::literals::chrono_literals;

std::string make_daytime_string()
{
  std::time_t now = std::time(0);
  return std::ctime(&now);
}

void start_server() {
  try
  {
    boost::asio::io_service io_service;
    std::string ip_addr("127.0.0.1");
    unsigned short port_number = 2737;
    tcp::acceptor acceptor(io_service, tcp::endpoint(boost::asio::ip::address_v4::from_string(ip_addr), port_number));
    tcp::socket socket(io_service);
    acceptor.accept(socket);

    while (true) {
      std::this_thread::sleep_for(3s);
      std::string message = make_daytime_string();
      boost::system::error_code error;
      boost::asio::write(socket, boost::asio::buffer(message), error);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

void start_server2() {
  try
  {
    boost::asio::io_service io_service;
    std::string ip_addr("127.0.0.1");
    unsigned short port_number = 2737;
    tcp::acceptor acceptor(io_service, tcp::endpoint(boost::asio::ip::address_v4::from_string(ip_addr), port_number));
    tcp::socket socket(io_service);
    acceptor.accept(socket);
    std::string message;
    while (true) {
      std::cout << "enter message ..." << std::endl;
      std::getline(std::cin, message);
      boost::system::error_code error;
      boost::asio::write(socket, boost::asio::buffer(message), error);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

//http://stackoverflow.com/questions/7556327/int-char-conversion-in-network-frames-with-c-boostasio
//http://stackoverflow.com/questions/22191725/how-to-support-mutiple-connections-in-the-tcp-server-via-boost-asio

void start_server3() {
  std::string ip_addr("127.0.0.1");
  unsigned short port_number = 2737;

  std::string data("string text literal");
  Server  &ser = Server::Instance(ip_addr, port_number);
  ser.run();
  std::string message;
  while (true) {
    std::cout << "Enter next message" << std::endl;
    std::getline(std::cin, message);
    if (message == "quit") {
      std::cout << "buy" << std::endl;
      break;
    }
    ser.write<std::string>(message);
  }
}

int main()
{
  //start_blocking_server();
  //start_async_server();

  std::string host("127.0.0.1");
  unsigned short port = 2737;

  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(host), port);
  boost::asio::io_service io_service;

  async_server async_serv(io_service, endpoint);
  std::thread service_thread([&io_service]()
  {
    try {
      io_service.run();
    }
    catch (std::exception  &ex) {
      std::cout << "exception occurs: " << ex.what() << std::endl;
    }
  });

  std::string message;
  while (std::getline(std::cin, message)) {
    if (message == "quit") {
      break;
    }
    async_serv.write(message);
  }
  service_thread.join();
  return 0;
}
