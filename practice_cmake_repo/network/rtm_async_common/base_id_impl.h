#pragma once
#include <boost/serialization/base_object.hpp>

#include "base_impl.h"

class base_task_id_impl : public base_task_impl
{
  friend boost::serialization::access;
  template<class Archive>
  void serialize(Archive  &ar, const unsigned int version) {
    ar  & boost::serialization::base_object<base_task_impl>(*this);
    ar & id_;
  }
public:
  base_task_id_impl();
  base_task_id_impl(base_task_impl::TASK_ID id);

  virtual ~base_task_id_impl();
  virtual void execute();
  virtual TASK_ID get_id() const;
private:
  TASK_ID id_;
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(base_task_id_impl)
BOOST_SERIALIZATION_SHARED_PTR(base_task_id_impl)
BOOST_CLASS_EXPORT_KEY(base_task_id_impl)
