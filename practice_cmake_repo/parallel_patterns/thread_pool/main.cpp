#include "threadsafe_queue.hpp"
#include "thread_pool.hpp"


void test_thread_pool() {

  auto start = std::chrono::high_resolution_clock::now();
  std::vector<ThreadPool::TaskFuture<void>> v;
  for (std::uint32_t i = 0u; i < 21u; ++i)
  {
    v.push_back(DefaultThreadPool::submitJob([]()
    {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }));
  }
  for (auto& item : v)
  {
    item.get();
  }
  auto diff = std::chrono::high_resolution_clock::now() - start;
  std::cout << "time duration: " << std::chrono::duration<double, std::milli>(diff).count() << std::endl;
}

//http://roar11.com/2016/01/a-platform-independent-thread-pool-using-c14/
auto main() -> int {
  
  //test_queue();
  test_thread_pool();

  return 0;
}