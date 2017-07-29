#pragma once

#include <type_traits>
#include <cinttypes>

#include <string>
#include <memory>

#include <boost/asio.hpp>



class Server {
public:
  explicit Server(const std::string  &ip_address, unsigned short port);
  ~Server();

  Server(const Server  &ser) = delete;
  Server & operator = (const Server  &ser) = delete;

  void run();
  void stop();

  const std::string  & getIpAddress() const;
  const unsigned short  &getPort() const;

  template<class T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
  void write(const T  &value);
  template<class T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
  void write(const T  &value);

  void setIpAddress(const std::string  &ip_address);
  void setPort(unsigned short port);

  static Server&  Instance(const std::string  &ip_address, unsigned short port);

private:
  //TO DO: add std::size_t for x64 address model
  void write_length(const std::uint32_t  &value);

  std::string ip_address_;
  unsigned short port_;
  boost::asio::io_service io_service_;
  //acceptor dtor should be called before socket
  std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
  std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
};

//TO DO: add type code specification
// can be done compile time recursive buffer initialization with htonl usage.
template<class T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
void Server::write(const T  &value) 
{
  std::size_t len = write_length(sizeof(T));
  boost::system::error_code error;
  std::size_t data = boost::asio::write(*socket_.get(), boost::asio::buffer(reinterpret_cast<char*>(&value), sizeof(value)), error);
}

template<class T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
void Server::write(const T  &value)
{
  write_length(value.length());
  boost::system::error_code error;
  size_t writtenData = boost::asio::write(*socket_.get(), boost::asio::buffer(value), error);
}
