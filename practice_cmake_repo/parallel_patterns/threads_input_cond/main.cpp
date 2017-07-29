#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>

//std::mutex mutex;
//std::condition_variable cv;
std::atomic_bool shutdown{ false };

//http://stackoverflow.com/questions/32206611/how-to-exit-from-a-background-thread-loop
//http://stackoverflow.com/questions/26516683/reusing-thread-in-loop-c

void thread_loop() {
  using namespace std::chrono_literals;
  while (!shutdown) {
    //int a = 0;
    //a++;
    std::cout << "do" << std::endl;
    std::this_thread::sleep_for(2s);
    //cv.wait_for(lock, std::chrono::seconds(15));
    //cv.wait(lock);
  }
  std::cout << "finish the loop" << std::endl;
}

auto main(int argc, char *argv[]) -> int {

  std::thread t1(thread_loop);

  int toggle = 0;
  do {
    std::cout << "do you want to stop ?" << std::endl;
    std::cin >> toggle;
  } while (toggle == 0);

  /*{
    std::lock_guard<std::mutex> lock(mutex);
    ready = true;
  }*/
  //cv.notify_one();
  shutdown = true;

  t1.join();

  return 0;
}

