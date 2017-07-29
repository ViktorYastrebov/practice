#pragma once

#include <string>

#include "base_task.h"

class message_impl;

class message : public base_task_requested {
public:
  message(const std::string  &msg);
  message(const std::shared_ptr<message_impl>  &msg_impl);
  virtual  ~message();
};