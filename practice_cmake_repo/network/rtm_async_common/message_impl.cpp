#include <iostream>

#include "message_impl.h"

//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/binary_oarchive.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

message_impl::message_impl()
  : base_task_id_impl(base_task_impl::MESSAGE)
{}

message_impl::message_impl(const std::string  &msg)
  :base_task_id_impl(base_task_impl::MESSAGE), msg_(msg)
{}

message_impl::~message_impl()
{}

void message_impl::execute() {
  std::cout << msg_ << std::endl;
}


const std::string  &message_impl::get_msg() const {
  return msg_;
}

std::string  &message_impl::get_msg() {
  return msg_;
}

BOOST_CLASS_EXPORT_IMPLEMENT(message_impl)
