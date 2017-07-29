#pragma once

#include <cinttypes>
#include <memory>
#include <string>

#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "expceptions.h"

class Client {
public:
  Client(const std::string  &host, const std::string  &port);
  ~Client();

  Client(const Client  &) = delete;
  Client & operator = (const Client  &) = delete;

  void connect();
  void connect(const std::string  &host, const std::string  &port);
  void disconnect();

  template<class T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
  T read();
  template<class T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
  T read();

private:
  std::uint32_t read_length();

  std::string host_;
  std::string port_;
  std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
  boost::asio::io_service io_service_;
};


template<class T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
T Client::read() {
  std::uint32_t expectedLength = read_length();
  assert(expectedLength == sizeof(T));

  char buffer[sizeof(T)];
  if (expectedLength > 0) {
    boost::system::error_code error;
    socket_->read_some(boost::asio::buffer(buffer, sizeof(T)), error);

    if (error == boost::asio::error::eof) {
      throw std::connection_closed("connection has been closed by peer");
    }else if (error) {
      throw boost::system::system_error(error);
    }
    return reinterpret_cast<T>(&buffer);
  } else {
    throw std::no_data_exception("expected length equals to 0 which means no data avaliable");
  }
  //should never happen;
  return T();
}

template<class T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
T Client::read() {
  std::uint32_t expectedLength = read_length();
  if (expectedLength > 0) {
    std::string data;
    data.resize(expectedLength);
    boost::system::error_code error;
    socket_->read_some(boost::asio::buffer(&data[0], expectedLength), error);

    if (error == boost::asio::error::eof) {
      throw std::connection_closed("connection has been closed by peer");
    }
    else if (error) {
      throw boost::system::system_error(error);
    }
    return data;
  } else {
    throw std::no_data_exception("expected length equals to 0 which means no data avaliable");
  }
  //should never happen;
  return std::string();
}