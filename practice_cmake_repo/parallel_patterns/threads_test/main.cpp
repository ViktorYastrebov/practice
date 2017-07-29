#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#include <vector>
#include <algorithm>

#include "thread_worker.h"
#include "conditional_variables.h"

thread_local unsigned int rage = 1;
std::mutex cout_mutex;

void increase_rage(const std::string& thread_name)
{
  ++rage;
  std::lock_guard<std::mutex> lock(cout_mutex);
  std::cout << "Rage counter for " << thread_name << ": " << rage << '\n';
}

void tls_example() {
  std::thread a(increase_rage, std::string("a")), b(increase_rage, std::string("b"));
  increase_rage("main");

  a.join();
  b.join();
}

void doTask(unsigned id) {
  std::cout << "doTask() id = " << id << std::endl;
}

void threads_container() {
  const int rounds = 10;
  std::vector<std::thread> threads;
  
  for (unsigned i = 0; i < rounds; ++i) {
    // spawn threads
    threads.push_back(std::thread(doTask, i));
  }
  // call join() on each thread in turn
  std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

thread_local int i = 0;

void f(int newval) {
  i = newval;
}

void g() {
  std::cout << i << std::endl;
}

void threadfunc(int id) {
  f(id);
  ++i;
  g();
}

void thread_local_test() {
  i = 9;
  std::thread t1(threadfunc, 1);
  std::thread t2(threadfunc, 2);
  std::thread t3(threadfunc, 3);

  t1.join();
  t2.join();
  t3.join();
  std::cout << i << std::endl;
}

//http://www.bogotobogo.com/cplusplus/multithreaded4_cplusplus11.php
//http://www.bogotobogo.com/cplusplus/C11/7_C11_Thread_Sharing_Memory.php
//http://www.bogotobogo.com/cplusplus/C11/3_C11_Threading_Lambda_Functions.php

//http://stackoverflow.com/questions/19422684/stdthread-management-usage-and-best-practice

//https://www.youtube.com/playlist?feature=c4-feed-u&list=PL1835A90FC78FF8BE

int main()
{
  //threads_container();
  //tls_example();
  //thread_local_test();
/*
  threads_worker thrdWorkers(2);
  std::string message;
  while (true) {
    std::getline(std::cin, message);
    if (message == "quit") {
      break;
    }
    thrdWorkers.setSharedData(message);
  }
  */
  conditional_variables cv;
  cv.setData(std::string("some data"));

  return 0;
}