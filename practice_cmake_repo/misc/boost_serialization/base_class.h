#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

class base_class {
  friend boost::serialization::access;
  template<class Archive>
  void serialize(Archive  &ar, const unsigned int version) {}
public:
  virtual ~base_class();
};

class base_class_id : public base_class
{
  friend boost::serialization::access;
  template<class Archive>
  void serialize(Archive  &ar, const unsigned int version)
  {
    //boost::serialization::void_cast_register<base_class_id, base_class>();
    ar & boost::serialization::base_object<base_class, base_class_id>(*this);
    ar & id_;
  }
public:
  base_class_id();
  base_class_id(int id);
  virtual int get_id() const;
  virtual ~base_class_id();
protected:
  int id_;
};
