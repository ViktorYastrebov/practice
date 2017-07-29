#pragma once

#include <string>
#include <memory>
#include <array>

class base_task_impl;

class request : public std::enable_shared_from_this<request>
{
public:
  static const std::size_t length = 4;

  request();
  //request(const std::shared_ptr<base_task_impl>  &task);
  request(const std::weak_ptr<base_task_impl>  &task);
  const std::string  &get_data() const;
  std::string  &get_data();
  void serialize();
  const std::array<unsigned char, length>  &get_size() const;
private:
  std::string data_;
  //std::shared_ptr<base_task_impl> task_;
  std::weak_ptr<base_task_impl> task_;
  std::array<unsigned char, length> serialized_length_;
};