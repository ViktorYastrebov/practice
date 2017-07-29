#include <sstream>

//#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/shared_ptr.hpp>

#include "request.hpp"
#include "base_impl.h"

#include <iostream>

#ifdef WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

request::request()
{}

request::request(const std::weak_ptr<base_task_impl>  &task)
  : task_(task)
{}

const std::string  &request::get_data() const {
  return data_;
}

std::string  &request::get_data() {
  return data_;
}

const std::array<unsigned char, request::length>  &request::get_size() const {
  return serialized_length_;
}

void request::serialize() {
  //should be moved to protocol
  std::ostringstream oss;

  /*boost::archive::binary_oarchive bin_oarch(oss);
  bin_oarch & task_;
  */
  std::shared_ptr<base_task_impl> task = task_.lock();
  boost::archive::text_oarchive text_oarch(oss);
  text_oarch & task;
  data_ = oss.str();

  std::cout << "serialize, length:"  << data_.length() << "data_ :" << data_ << std::endl;

  std::uint32_t len = htonl(data_.length());
  serialized_length_[0] = len >> 0;
  serialized_length_[1] = len >> 8;
  serialized_length_[2] = len >> 16;
  serialized_length_[3] = len >> 24;
}
