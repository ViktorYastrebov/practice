#pragma once

#include <thread>
#include <mutex>
#include <string>
#include <vector>

#include <cinttypes>

// point is: wait until the data is no avaliable
// barrier patter should be used
// then send all data

class threads_worker {
public:
  explicit threads_worker(std::int32_t  max_threads);
  ~threads_worker();

  void threadJob();

  void setSharedData(const std::string  &msg);
  std::mutex  &getMutex();
  const std::mutex  &getMutex() const;
  void stop();
private:
  std::mutex mutex_;
  std::vector<std::thread> threads_;
  std::string shared_data_;
  bool continue_;
};