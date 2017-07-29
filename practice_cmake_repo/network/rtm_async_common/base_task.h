#pragma once

class request;
class response;

#include "base_impl.h"

class base_task {
public:
  virtual ~base_task() = 0;
  virtual std::shared_ptr<request> get_request() const = 0;
  virtual std::shared_ptr<response> get_response() const = 0;
  virtual bool has_responce() const = 0;
  virtual std::shared_ptr<base_task_impl> task() const = 0;
};

class base_task_requested : public base_task {
public:
  base_task_requested(const std::shared_ptr<base_task_impl>  &task);
  virtual ~base_task_requested();
  virtual std::shared_ptr<request> get_request() const;
  virtual std::shared_ptr<response> get_response() const;
  virtual bool has_responce() const;
  virtual std::shared_ptr<base_task_impl> task() const;
protected:
  std::shared_ptr<base_task_impl> task_;
  std::shared_ptr<request> request_;
};

class base_task_responded : public base_task_requested {
public:
  base_task_responded(const std::shared_ptr<base_task_impl>  &task);
  virtual ~base_task_responded();
  virtual std::shared_ptr<request> get_request() const;
  virtual std::shared_ptr<response> get_response() const;
  virtual bool has_responce() const;
  virtual void post_action();
protected:
  std::shared_ptr<response> response_;
};
