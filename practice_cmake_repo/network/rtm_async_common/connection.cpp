#include "connection.hpp"
#include <functional>

#include "request.hpp"
#include "response.hpp"

#include "task_builder.h"
#include "base_id_impl.h"

connection::connection(boost::asio::io_service  &io_service)
  :socket_(io_service)
{}

connection::~connection()
{
  socket_.close();
}


boost::asio::ip::tcp::socket  & connection::get_socket() {
  return socket_;
}

void connection::init_recieving() {
  std::shared_ptr<base_task> recieved = std::make_shared<base_task_responded>(std::make_shared<base_task_id_impl>());
  read_pool_.push_back(recieved);
  std::shared_ptr<response> respon = recieved->get_response();

  auto self(shared_from_this());
  boost::asio::async_read(socket_,
    boost::asio::buffer(respon->get_size()),
    boost::asio::transfer_exactly(response::length),
    std::bind(&connection::handle_read_request_length, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void connection::handle_read_request_length(const boost::system::error_code  &error, std::size_t bytes_transferred /*, char  *network_length*/) {
  if (!error) {
    std::shared_ptr<base_task> &front = read_pool_.front();

    std::shared_ptr<response> respon = front->get_response();
    respon->prepare_buffer();
    //std::size_t length = respon->get_buffer().length();
    std::size_t length = respon->get_buffer().size();

    boost::asio::async_read(socket_,
      boost::asio::buffer(&respon->get_buffer()[0], length),
      boost::asio::transfer_exactly(length),
      std::bind(&connection::handle_read_request_data, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  }
}

//the point is: reading stack should be: [1](reading) -> [2](process) -> [1](reading) -> [2](reading) -> [3](process) -> [2]
void connection::handle_read_request_data(const boost::system::error_code  &error, std::size_t bytes_transferred /*, char  *data*/) {
  std::shared_ptr<base_task> front = read_pool_.front();
  read_pool_.pop_front();
  std::shared_ptr<base_task> read_task = task_builder::instance().create(front->get_response()->deserialize());

  read_task->task()->execute();

  if (read_task->has_responce()) {
    //requests_pool_.push_back(read_task);
    responces_pool_.push_back(read_task);
    std::shared_ptr<request>  req = read_task->get_request();
    req->serialize();
    boost::asio::async_write(socket_, boost::asio::buffer(req->get_size()),
      //std::bind(&connection::handle_write_request_length, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
      std::bind(&connection::handle_write_response_length, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  } else {
    init_recieving();
  }
}

void connection::send(const std::shared_ptr<base_task>  &task) {

  requests_pool_.push_back(task);
  std::shared_ptr<request> req = task->get_request();
  req->serialize();
  boost::asio::async_write(socket_,
    boost::asio::buffer(req->get_size()),
    boost::asio::transfer_exactly(request::length),
    std::bind(&connection::handle_write_request_length, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void connection::handle_write_request_length(const boost::system::error_code  &error, std::size_t bytes_transferred) {
  if (!error) {
    std::shared_ptr<base_task>  &last = requests_pool_.front();
    std::shared_ptr<request> req = last->get_request();
    std::size_t length = req->get_data().length();
    boost::asio::async_write(socket_,
      boost::asio::buffer(req->get_data(), length),
      boost::asio::transfer_exactly(length),
      std::bind(&connection::handle_write_request_data, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  } else {
    throw std::runtime_error(error.message());
  }
}

void connection::handle_write_request_data(const boost::system::error_code  &error, std::size_t bytes_transferred) {
  std::shared_ptr<base_task> last = requests_pool_.front();
  requests_pool_.pop_front();
  if (error) {
    throw std::runtime_error(error.message());
  }

  //if it's client side it should not happened
  if (last->has_responce()) {
    //TODO : can be just base_responded!!!
    responces_pool_.push_back(last);
    //std::shared_ptr<base_task> general = std::make_shared<base_task_responded>(std::make_shared<base_task_id_impl>());
    //responces_pool_.push_back(general);
    std::shared_ptr<response>  respon = last->get_response();
    boost::asio::async_read(socket_, boost::asio::buffer(respon->get_size()),
      std::bind(&connection::handle_read_response_length, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  }
}

void connection::handle_read_response_length(const boost::system::error_code  &error, std::size_t bytes_transferred) {
  std::shared_ptr<base_task>  &last = responces_pool_.front();
  std::shared_ptr<response> respon = last->get_response();
  if (error) {
    throw std::runtime_error(error.message());
  }
  respon->prepare_buffer();
  //std::size_t length = respon->get_buffer().length();
  std::size_t length = respon->get_buffer().size();
  boost::asio::async_read(socket_, boost::asio::buffer(&respon->get_buffer()[0], length),
    std::bind(&connection::handle_read_response_data, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void connection::handle_read_response_data(const boost::system::error_code  &error, std::size_t bytes_transferred) {
  std::shared_ptr<base_task> respon_task = responces_pool_.front();
  responces_pool_.pop_front();
  if (error) {
    throw std::runtime_error(error.message());
  }
  //there can be bind the post action.
  std::shared_ptr<base_task> resp_base = task_builder::instance().create(respon_task->get_response()->deserialize());
  std::shared_ptr<base_task_responded> responded = std::static_pointer_cast<base_task_responded>(resp_base);
  responded->post_action();
}


void connection::handle_write_response_length(const boost::system::error_code  &error, std::size_t bytes_transferred) {
  if (!error) {
    std::shared_ptr<base_task>  &last = responces_pool_.front();
    std::shared_ptr<request> req = last->get_request();
    std::size_t length = req->get_data().length();
    boost::asio::async_write(socket_,
      boost::asio::buffer(req->get_data(), length),
      boost::asio::transfer_exactly(length),
      std::bind(&connection::handle_write_response_data, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  }
  else {
    throw std::runtime_error(error.message().c_str());
  }
}

void connection::handle_write_response_data(const boost::system::error_code  &error, std::size_t bytes_transferred) {
  if (!error) {
    responces_pool_.pop_front();
  } else {
    throw std::runtime_error(error.message().c_str());
  }
  init_recieving();
}

