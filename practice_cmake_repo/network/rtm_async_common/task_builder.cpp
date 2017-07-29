#include "task_builder.h"

#include "message_impl.h"
#include "os_info_impl.h"
#include "message.h"
#include "os_info.h"


task_builder  &task_builder::instance() {
  static task_builder instance;
  return instance;
}

task_builder::task_builder()
{
  task_map_[base_task_impl::MESSAGE] = [](const std::shared_ptr<base_task_impl>  &base_impl) { return std::make_shared<message>(std::dynamic_pointer_cast<message_impl>(base_impl)); };
  task_map_[base_task_impl::OS_INFO] = [](const std::shared_ptr<base_task_impl>  &base_impl) { return std::make_shared<os_info>(std::dynamic_pointer_cast<os_info_impl>(base_impl)); };
}

std::shared_ptr<base_task>  task_builder::create(const std::shared_ptr<base_task_impl>  &base_impl) const {
  map_t::const_iterator it = task_map_.find(base_impl->get_id());
  if (it != task_map_.end()) {
    return it->second(base_impl);
  }
  return std::shared_ptr<base_task>();
}
