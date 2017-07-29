#include "os_info_impl.h"
#include "os_info.h"

os_info::os_info()
  :base_task_responded(std::make_shared<os_info_impl>())
{}

os_info::os_info(const std::shared_ptr<os_info_impl>  &from)
  : base_task_responded(from)
{}

os_info::~os_info()
{}


void os_info::post_action() {
  std::shared_ptr<os_info_impl> osi_impl = std::static_pointer_cast<os_info_impl>(task_);
  if (osi_impl) {
    std::cout << "OS description is: " <<  osi_impl->get_description() << std::endl;
  }
}