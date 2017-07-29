#pragma once

#include <string>
#include "base_class.h"

class derived1 : public base_class_id
{
  friend boost::serialization::access;
  template<class Archive>
  void serialize(Archive  &ar, const unsigned int version)
  {
    //boost::serialization::void_cast_register<derived1, base_class_id>();
    ar & boost::serialization::base_object<base_class_id, derived1>(*this);
    ar & data_;
  }
public:
  derived1();
  derived1(const std::string  &value);
  void set(const std::string  &value);
  const std::string  &get() const;
private:
  std::string data_;
};