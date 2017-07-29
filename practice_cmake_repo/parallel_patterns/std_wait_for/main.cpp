#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <iostream>
#include <string>
#include <deque>

class task_sheduler {
public:
  task_sheduler();
  task_sheduler(const task_sheduler  &ts) = default;
  task_sheduler & operator  = (const task_sheduler  &ts) = default;
  ~task_sheduler();

  void next_task();
  void push_back(int value);

  void start();
  void stop();
  void restart();
#if 0
  void next_task_runner();
#endif

  static constexpr std::chrono::seconds duration{ 5 };

private:
  std::deque<int> values_;
  std::atomic<bool> flag;
private:
  std::mutex cv_m;
  std::condition_variable_any cv;
private:
  std::thread th_;
  //std::thread helper_;
};

constexpr std::chrono::seconds task_sheduler::duration;

task_sheduler::task_sheduler()
  :values_(), flag(false), cv_m(), cv()//, helper_()
{}

task_sheduler::~task_sheduler()
{
  th_.join();
  //if (helper_.joinable()) {
  //  helper_.join();
  //}
}

#if 0
void task_sheduler::next_task_runner() {
  th_.join();
  start();
}
#endif

//It's a question how to start new loop/waiting if there was no any interruption
void task_sheduler::next_task() {
#if 0
  bool res = false;
  {
    std::unique_lock<std::mutex> lk(cv_m);
    res = cv.wait_for(lk, duration, [this] {return flag; });
  }
  if (res) {
    std::cout << "Interrupted" << std::endl;
  }
  else {
    //if (helper_.joinable()) {
    //  helper_.join();
    //}
    std::cerr << "time out is over, continue new counting, flag : " << std::boolalpha << flag << std::endl;
    //helper_ = std::thread(&task_sheduler::next_task_runner, this);
    restart();
  }
#else
  std::atomic<bool> res = false;
  {
    std::unique_lock<std::mutex> lk(cv_m);
    res = cv.wait_for(lk, duration, [this] {return flag.load(); });
  }
  if (res.load()) {
    std::cout << "Interrupted" << std::endl;
  } else {
    std::cerr << "time out is over, continue new counting, flag : " << std::boolalpha << flag << std::endl;
    std::thread(&task_sheduler::restart, this).detach();
  }
#endif
}

void task_sheduler::push_back(int value) {
  {
    std::lock_guard<std::mutex> lk(cv_m);
    //flag = true;
    flag.store(true);
  }
  cv.notify_one();
  values_.push_back(value);
  restart();
}

void task_sheduler::stop() {
  std::lock_guard<std::mutex> lk(cv_m);
  //flag = true;
  flag.store(true);
  cv.notify_one();
}

void task_sheduler::start() {
  th_ = std::thread(&task_sheduler::next_task, this);
  //std::thread(&task_sheduler::next_task, this).detach();
}

void task_sheduler::restart() {
  th_.join();
  flag.store(false);
  start();
}

auto main() -> int {
  task_sheduler ts;

  ts.start();
  std::cout << "enter exit or integer value" << std::endl;
  std::cout << "seconds : " << task_sheduler::duration.count() << std::endl;
  std::string value;

  while (std::getline(std::cin, value)) {
    std::cout << "value : " << value << std::endl;
    if (value == "exit") {
      ts.stop();
      break;
    }
    int ival = std::stoi(value);
    ts.push_back(ival);
  }

  return 0;
}