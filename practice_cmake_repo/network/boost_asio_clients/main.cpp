
/*
int main(int argc, char*argv[]) {

  return 0;
}*/


#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <cinttypes>

#include "tcp_client.h"

//using boost::asio;
//int i;
//short s;
//char data[data_size]; // data_size is defined elsewhere
//boost::array<asio::mutable_buffer, 3> bufs = {
//  asio::buffer(&i, 4),
//  asio::buffer(&s, 2),
//  asio::buffer(data, data_size)
//};
//asio::read(socket, buffer(bufs));


void start_client() {
  using boost::asio::ip::tcp;
  try
  {
    std::string host("127.0.0.1");
    std::string server("2737");

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host, server);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);
    std::size_t expectedLength = 0;
    while (true)
    {
      boost::system::error_code error;
      //std::size_t len = boost::asio::read_until(socket, buff, '\n', error);
      //std::size_t len = boost::asio::read_size_helper(buff, sizeof(std::uint32_t), error);
      std::uint32_t net_length = 0;
      socket.read_some(boost::asio::buffer(reinterpret_cast<char*>(&net_length), sizeof(net_length)), error);
      if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw boost::system::system_error(error); // Some other error.

      std::uint32_t expectedLength = ntohl(net_length);
      if (expectedLength > 0) {
        std::string data;
        data.resize(expectedLength);
        socket.read_some(boost::asio::buffer(&data[0], expectedLength), error);

        if (error == boost::asio::error::eof)
          break; // Connection closed cleanly by peer.
        else if (error)
          throw boost::system::system_error(error);

        std::cout << "expected length = " << expectedLength << std::endl;
        std::cout << "length = " << data.length() << " data = " << data << std::endl;
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

int main(int argc, char* argv[])
{
  std::string host("127.0.0.1");
  std::string port("2737");
  Client client(host, port);
  try {
    client.connect();
  } catch (std::connection_expception  &error) {
    std::cout << "connection failure ..." << std::endl;
    std::cout << "error message = " << error.what() << std::endl;
    return 0;
  }
 
  while (true) {
    try {
      std::string message = client.read<std::string>();
      std::cout << "sent message:" << message << std::endl;
    } catch (std::no_data_exception  &no_data) {
      //std::cout << no_data.what() << std::endl;
    } catch (std::connection_closed  &closed) {
      std::cout << closed.what() << std::endl;
      return 0;
    } catch (boost::system::system_error  &sys_error) {
      std::cout << "error code = " << sys_error.code() << ", what = " << sys_error.what() << std::endl;
    }
  }

  return 0;
}