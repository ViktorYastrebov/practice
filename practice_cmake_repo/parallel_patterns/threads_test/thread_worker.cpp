#include "thread_worker.h"

#include <iostream>

threads_worker::threads_worker(std::int32_t  max_threads)
  :continue_(true)
{
  mutex_.lock();
  for (std::int32_t i = 0; i < max_threads; ++i) {
    threads_.emplace_back(std::thread(&threads_worker::threadJob, this));
  }
}

threads_worker::~threads_worker() {}

void threads_worker::threadJob() {
  while (continue_) {
    std::lock_guard<std::mutex> lc(mutex_);
    std::cout << "thread_id = " << std::this_thread::get_id() << "data = " << shared_data_ << std::endl;
  }
}

std::mutex  &threads_worker::getMutex() {
  return mutex_;
}

const std::mutex  &threads_worker::getMutex() const {
  return mutex_;
}

void threads_worker::setSharedData(const std::string  &msg) {
  mutex_.unlock();
  shared_data_ = msg;
  mutex_.lock();
}

void threads_worker::stop() {
  continue_ = false;
}