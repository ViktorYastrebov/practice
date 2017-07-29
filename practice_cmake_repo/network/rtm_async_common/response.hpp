#pragma once

//#include <string>
#include <vector>
#include <memory>
#include <array>

class base_task_impl;

class response : public std::enable_shared_from_this<response>
{
public:
  static const std::size_t length = 4;
  response();
  //response(const std::shared_ptr<base_task_impl>  &task);
  response(const std::weak_ptr<base_task_impl>  &task);

  void prepare_buffer();

  //const std::string &get_buffer() const;
  //std::string  &get_buffer();
  const std::vector<char>  &get_buffer() const;
  std::vector<char>  &get_buffer();

  const std::array<unsigned char, length>  &get_size() const;
  std::array<unsigned char, length>  &get_size();
  //std::vector<char>  &get_size();
  //const std::vector<char> &get_size() const;

  std::shared_ptr<base_task_impl> deserialize();

private:
  //std::string data_;
  std::vector<char> data_;
  std::array<unsigned char, length> deserialized_length_;
  //std::shared_ptr<base_task_impl> task_;
  std::weak_ptr<base_task_impl> task_;
};
