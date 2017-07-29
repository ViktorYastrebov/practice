#pragma once

#include <map>
#include <functional>

#include "base_task.h"

class task_builder {
public:
  task_builder();
  std::shared_ptr<base_task>  create(const std::shared_ptr<base_task_impl>  &base_impl) const;
  static task_builder  &instance();
private:
  typedef std::map<base_task_impl::TASK_ID, std::function<std::shared_ptr<base_task>(const std::shared_ptr<base_task_impl>  &base_impl)>> map_t;
  map_t task_map_;
};