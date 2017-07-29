#include <string>
#include <sstream>

//#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "response.hpp"
#include "base_impl.h"

#include <iostream>

#ifdef WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

response::response()
  :deserialized_length_({0,0,0,0})
{}

response::response(const std::weak_ptr<base_task_impl>  &task)
  :task_(task)
{}

const std::array<unsigned char, response::length>  &response::get_size() const {
  return deserialized_length_;
}

std::array<unsigned char, response::length>  &response::get_size() {
  return deserialized_length_;
}

std::shared_ptr<base_task_impl> response::deserialize() {
  //std::cout << "deserialize, data :" << data_ << std::endl;
  //std::cout << "vector value :" << data_ << std::endl;
  std::copy(data_.begin(), data_.end(), std::ostream_iterator<char>(std::cout));
  std::string value(&data_[0], data_.size());
  std::cout << "deserialize, data :" << value << std::endl;
  std::istringstream iss(value);
  //boost::archive::binary_iarchive bin_iarch(iss);
  //bin_iarch  & task_;
  std::shared_ptr<base_task_impl> task;
  boost::archive::text_iarchive text_iarch(iss);
  text_iarch  & task;
  return task_.lock();
}

void response::prepare_buffer() {

  std::size_t length = 0;
  length += deserialized_length_[0] << 0;
  length += deserialized_length_[1] << 8;
  length += deserialized_length_[2] << 16;
  length += deserialized_length_[3] << 24;
  std::size_t reserved_length = ntohl(length);
  std::cout << "prepare_buffer :" << reserved_length << std::endl;
  //data_.resize(reserverd_length, '\0');
  //data_ = std::string(reserved_length, '\0');
  data_.resize(reserved_length, '\0');
}

//const std::string &response::get_buffer() const {
const std::vector<char>& response::get_buffer() const {
  return data_;
}

//std::string  &response::get_buffer() {
std::vector<char>& response::get_buffer() {
  return data_;
}
