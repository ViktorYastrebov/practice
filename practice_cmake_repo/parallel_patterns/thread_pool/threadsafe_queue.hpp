#include <thread>
#include <deque>
#include <mutex>
#include <memory>
#include <exception>

template<class T>
class ThreadSafeDeque {
public:
  ThreadSafeDeque();
  ThreadSafeDeque(const ThreadSafeDeque  &tsd);
  ThreadSafeDeque  & operator = (const ThreadSafeDeque  &tsd) = delete;
  void push(T value);
  std::shared_ptr<T>  pop();
  void pop(T &ref);
  bool empty() const;
private:
  mutable std::mutex mutex_;
  std::deque<T> deque_;
};

template<class T>
ThreadSafeDeque<T>::ThreadSafeDeque() {}

template<class T>
ThreadSafeDeque<T>::ThreadSafeDeque(const ThreadSafeDeque<T>  &tsd) {
  std::lock_guard<std::mutex> lock(tsd.mutex_);
  deque_ = tsd.deque_;
}

template<class T>
void ThreadSafeDeque<T>::push(T value) {
  std::lock_guard<std::mutex> lock(mutex_);
  deque_.push_back(value);
}

template<class T>
std::shared_ptr<T> ThreadSafeDeque<T>::pop() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (deque_.empty()) {
    throw std::runtime_error("empty deque");
  }
  std::shared_ptr<T> res(std::make_shared<T>(deque_.back()));
  deque_.pop_back();
  return res;
}

template<class T>
void ThreadSafeDeque<T>::pop(T  &value) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (deque_.empty()) {
    throw std::runtime_error("empty deque");
  }
  value = deque_.back();
  deque_.pop_back();
}

template<class T>
bool ThreadSafeDeque<T>::empty() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return deque_.empty();
}
//=========================================================
#include <iostream>

template<class T>
void write_data(ThreadSafeDeque<T>  &queue, const int interval) {
  for (int i = 0; i < interval; ++i) {
    queue.push(i);
  }
}

template<class T>
void read_func(ThreadSafeDeque<T>  &queue, const int interval) {
  for (int i = 0; i < interval; ++i) {
    T value;
    try {
      queue.pop(value);
    }
    catch (std::exception  &ex) {
      std::cout << "ex = " << ex.what() << std::endl;
    }
    std::cout << "value = " << value << std::endl;
  }
}

void test_queue() {
  const int size = 20;
  ThreadSafeDeque<int> tfq;
  try {
    std::thread t1(write_data<int>, std::ref(tfq), size);
    std::thread t2(read_func<int>, std::ref(tfq), size);
    t1.join();
    t2.join();
  }
  catch (std::exception  &ex) {
    std::cout << ex.what() << std::endl;
  }
}