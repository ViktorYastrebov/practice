#pragma once

#include <string>
#include "base_id_impl.h"

class os_info_impl : public base_task_id_impl
{
  friend boost::serialization::access;
  template<class Archive>
  void serialize(Archive  &ar, const unsigned int version) {
    ar & boost::serialization::base_object<base_task_id_impl>(*this);
    ar & description_;
  }

public:
  os_info_impl();
  virtual ~os_info_impl();

  virtual void execute();
  const std::string  &get_description() const;
  std::string  &get_description();
protected:
  std::string get_os_description() const;
private:
  std::string description_;
};

BOOST_CLASS_EXPORT_KEY(os_info_impl)
BOOST_SERIALIZATION_SHARED_PTR(os_info_impl)
