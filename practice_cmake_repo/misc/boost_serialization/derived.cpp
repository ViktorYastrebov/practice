#include "derived.h"

#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>

derived1::derived1()
  :base_class_id(2)
{}

derived1::derived1(const std::string  &value)
  : base_class_id(2), data_(value)
{}

void derived1::set(const std::string  &value) {
  data_ = value;
}
const std::string  &derived1::get() const {
  return data_;
}
