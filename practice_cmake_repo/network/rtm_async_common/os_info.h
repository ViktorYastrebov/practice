#pragma once

#include "base_task.h"

class os_info_impl;

class os_info : public base_task_responded
{
public:
  os_info();
  //it may be replaced for base_task_responded::from()
  os_info(const std::shared_ptr<os_info_impl>  &from);
  virtual ~os_info();
  virtual void post_action();

  /*virtual void execute();
  virtual bool has_responce() const;

  const std::string  &get_description() const;
  std::string  &get_description();
  void set(const std::string  &desciption);
  
  virtual std::shared_ptr<base_task> create();
protected:
  os_info();
  virtual ~os_info();
  */
};