#pragma once

#include <memory>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>

class base_task_impl : public std::enable_shared_from_this<base_task_impl>
{
  friend boost::serialization::access;
  template<class Archive>
  void serialize(Archive  &ar, const unsigned int version)
  {}

public:
  enum TASK_ID : int
  {
    BASE = 0,
    BASE_ID,
    MESSAGE,
    OS_INFO
  };

  virtual ~base_task_impl();
  virtual void execute() = 0;
  virtual TASK_ID get_id() const = 0;
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(base_task_impl)
BOOST_SERIALIZATION_SHARED_PTR(base_task_impl)
BOOST_CLASS_EXPORT_KEY(base_task_impl)
