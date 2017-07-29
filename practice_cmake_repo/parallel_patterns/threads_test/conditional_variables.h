#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>


//http://en.cppreference.com/w/cpp/thread/condition_variable/notify_all
//it's better solution

class conditional_variables
{
public:
  conditional_variables();
  ~conditional_variables();

  void setData(const std::string  &data);

  void worker_thread();

  void run_test();

private:
  std::mutex mutex_;
  std::condition_variable cv_;
  std::string data_;


  //bool get_ready() const;

  bool ready;  //= false;
  bool processed;// = false;
};
