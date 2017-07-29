#include "base_id_impl.h"

//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/binary_oarchive.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

base_task_id_impl::base_task_id_impl()
  :id_(BASE_ID)
{}

base_task_id_impl::base_task_id_impl(base_task_impl::TASK_ID id)
  : id_(id)
{}

base_task_id_impl::~base_task_id_impl()
{}

void base_task_id_impl::execute()
{}

base_task_impl::TASK_ID base_task_id_impl::get_id() const {
  return id_;
}

BOOST_CLASS_EXPORT_IMPLEMENT(base_task_id_impl)
