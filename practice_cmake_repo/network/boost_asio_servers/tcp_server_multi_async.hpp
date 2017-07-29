#pragma once


#include <string>
#include <boost/asio.hpp>
#include <memory>

#include <type_traits>
#include <cinttypes>
#include <functional>

#include <list>

#include <iostream>

//may be need a queue

//see : http://www.boost.org/doc/libs/1_47_0/doc/html/boost_asio/example/chat/

class connection : public std::enable_shared_from_this<connection> {
public:
  explicit connection(boost::asio::io_service  &io_service)
    : socket_(io_service)
  {}
  ~connection()
  {}

  void stop();

  template<class T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
  void write(const T  &value);
  
  boost::asio::ip::tcp::socket  & socket();
private:

  //about the sending different types. It's wiser to use boost::serialization with inheritance for different protocols.
  // do not invent the bike!!! :)

  template<class T, typename std::enable_if<std::is_same<T, std::string>::value>::type* =  nullptr>
  void write_prepared(const T  &value, std::int32_t  lenght);

  template<class T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
  void handle_write_lenght(const boost::system::error_code  &error, std::size_t bytes_transferred, const T  &value);

  void handle_write_data(const boost::system::error_code  &error, std::size_t bytes_transferred) {
    if (error) {
      throw std::runtime_error("data has not been sent");
    }
  }

  boost::asio::ip::tcp::socket socket_;
};

template<class T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
void connection::write_prepared(const T  &value, std::int32_t length) {

  static const int bufferLength = 4;
  char buffer[bufferLength];
  std::uint32_t len = htonl(length);

  buffer[0] = len >> 0;
  buffer[1] = len >> 8;
  buffer[2] = len >> 16;
  buffer[3] = len >> 24;
  boost::asio::async_write(socket_, boost::asio::buffer(buffer, bufferLength),
    std::bind(&connection::handle_write_lenght<T>, shared_from_this(), std::placeholders::_1, std::placeholders::_2, value));
}

template<class T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
void connection::handle_write_lenght(const boost::system::error_code  &error, std::size_t bytes_transferred, const T  &value) {
  if (!error) {
    boost::asio::async_write(socket_, boost::asio::buffer(value),
      std::bind(&connection::handle_write_data, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  }
}

template<class T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
void connection::write(const T  &value) {
  //write_prepared<T>(value, value.length());
  boost::asio::async_write(socket_, boost::asio::buffer(value),
    std::bind(&connection::handle_write_data, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

class async_server {
public:
  async_server(boost::asio::io_service& io_service, const boost::asio::ip::tcp::endpoint &endpoint)
    : acceptor_(io_service, endpoint)
    //,socket_(io_service)
  {
    do_accept();
  }
  
  void write(const std::string  &data) {
    try {
      for (auto &i : connection_pool_) {
        //std::string thread_data = data;
        //i->write<std::string>(thread_data);
        i->write<std::string>(data);
      }
    }
    catch (std::exception  &ex)
    {
      std::cout << "error = " << ex.what() << std::endl;
    }
  }

private:

  void accept_handler(const boost::system::error_code& error, std::shared_ptr<connection>  con) {
    if (!error) {
      std::cout << "connected ..." << std::endl;
      connection_pool_.push_back(con);
      do_accept();
    }
  }

  void do_accept() {
    std::shared_ptr<connection> new_connection = std::make_shared<connection>(acceptor_.get_io_service());
    //connection_pool_.push_back(new_connection);
    acceptor_.async_accept(new_connection->socket(), std::bind(&async_server::accept_handler, this, std::placeholders::_1, new_connection));
    /*
    acceptor_.async_accept(socket_,
      [this](boost::system::error_code ec)
      {
        if (!ec) {
          //connection_pool_.emplace_back(std::make_shared<connection>(socket_.get_io_service()));
          connection_pool_.push_back(std::make_shared<connection>(socket_.get_io_service()));
        }
      });*/
  }

  boost::asio::ip::tcp::acceptor acceptor_;
  //tcp::socket socket_;
  std::list<std::shared_ptr<connection> > connection_pool_;
};
