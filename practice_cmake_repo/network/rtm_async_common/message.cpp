#include "message_impl.h"
#include "message.h"

message::message(const std::string  &msg)
  : base_task_requested(std::make_shared<message_impl>(msg))
{}

message::message(const std::shared_ptr<message_impl>  &msg_impl)
  : base_task_requested(msg_impl)
{}

message::~message()
{}