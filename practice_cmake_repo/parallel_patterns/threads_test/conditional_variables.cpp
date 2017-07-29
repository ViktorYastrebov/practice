#include "conditional_variables.h"

#include <iostream>

conditional_variables::conditional_variables()
  :mutex_(), cv_(), data_(), ready(false), processed(false)
{}

conditional_variables::~conditional_variables()
{}

void conditional_variables::setData(const std::string  &data) {
  data_ = data;
}

void conditional_variables::worker_thread() {
  // Wait until main() sends data
  std::unique_lock<std::mutex> lk(mutex_);
  cv_.wait(lk, [this]() { return ready;} );

  // after the wait, we own the lock.
  //std::cout << "Worker thread is processing data\n";
  //data += " after processing";

  // Send data back to main()
  std::cout << data_ << std::endl;
  processed = true;
  //std::cout << "Worker thread signals data processing completed\n";

  // Manual unlocking is done before notifying, to avoid waking up
  // the waiting thread only to block again (see notify_one for details)
  lk.unlock();
  cv_.notify_one();
  //cv_.notify_all
}

void conditional_variables::run_test() {
  std::thread worker1(&conditional_variables::worker_thread, this);
  std::thread worker2(&conditional_variables::worker_thread, this);
  std::thread worker3(&conditional_variables::worker_thread, this);
  std::thread worker4(&conditional_variables::worker_thread, this);

  //data_ = "example data string";
  {
    std::lock_guard<std::mutex> lk(mutex_);
    ready = true;
  }
  cv_.notify_all();

  {
    std::unique_lock<std::mutex> lk(mutex_);
    cv_.wait(lk, [this]() { return processed; } );
  }
  worker1.join();
  worker2.join();
  worker3.join();
  worker4.join();
}
