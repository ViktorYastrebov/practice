#include "base_class.h"

#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>

base_class::~base_class()
{}

base_class_id::base_class_id()
  :id_(1)
{}

base_class_id::base_class_id(int id)
  :id_(id)
{}

base_class_id::~base_class_id()
{}

int base_class_id::get_id() const {
  return id_;
}
