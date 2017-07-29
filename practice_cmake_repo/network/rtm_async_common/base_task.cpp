#include "base_task.h"
#include "request.hpp"
#include "response.hpp"

//base
base_task::~base_task()
{}

//requested
base_task_requested::base_task_requested(const std::shared_ptr<base_task_impl>  &task)
  :base_task(), task_(task), request_(std::make_shared<request>(task_))
{}

base_task_requested::~base_task_requested()
{}

std::shared_ptr<request> base_task_requested::get_request() const {
  return request_;
}

std::shared_ptr<response> base_task_requested::get_response() const {
  return std::shared_ptr<response>();
}

bool base_task_requested::has_responce() const {
  return false;
}

std::shared_ptr<base_task_impl> base_task_requested::task() const {
  return task_;
}



//responded
base_task_responded::base_task_responded(const std::shared_ptr<base_task_impl>  &task)
  :base_task_requested(task), response_(std::make_shared<response>(task))
{}

base_task_responded::~base_task_responded()
{}

std::shared_ptr<request> base_task_responded::get_request() const {
  return base_task_requested::get_request();
}

std::shared_ptr<response> base_task_responded::get_response() const {
  return response_;
}

bool base_task_responded::has_responce() const {
  return (bool)response_;
}

void base_task_responded::post_action() {
}
