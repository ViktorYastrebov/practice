#pragma once

#include <string>
#include "base_id_impl.h"

class message_impl : public base_task_id_impl
{
  friend boost::serialization::access;
  template<class Archive>
  void serialize(Archive  &ar, const unsigned int version) {
    ar & boost::serialization::base_object<base_task_id_impl>(*this);
    ar & msg_;
  }

public:
  message_impl();
  message_impl(const std::string  &msg);
  virtual ~message_impl();
  virtual void execute();

  const std::string  &get_msg() const;
  std::string  &get_msg();
private:
  std::string msg_;
};

BOOST_CLASS_EXPORT_KEY(message_impl)
BOOST_SERIALIZATION_SHARED_PTR(message_impl)
