#include <thread>
#include <chrono>

#include <random>
#include <vector>
#include <future>

#include <algorithm>
#include <numeric>

//temporary for testing
template<typename T>
T sum(const T &a, const T  &b) {
  return a + b;
}

template<typename T>
void sum2(const T *arr, std::size_t l, T &res) {
  std::size_t idx = 0;
  while (idx < l) {
    res += arr[idx];
    ++idx;
  }
}

template<typename T>
T linear_sum(const T  *array, std::size_t length) {
  T sum = 0;
  for (int i = 0; i < length; ++i) {
    sum += array[i];
  }
  return sum;
}

//currently its the tree parallel base implementation.
//In general case there is possible to implement with recursive thread invoking.

template<typename T>
T reduceSum(const T  *array, std::size_t seed_size, std::size_t length) {

  std::size_t calc_per_thread = length / seed_size;

  std::vector<T> res(seed_size);
  std::vector<std::thread> thread_pool;

  for (int i = 0; i < seed_size; ++i) {
    thread_pool.emplace_back(std::thread(sum2<int>, &array[i*calc_per_thread], calc_per_thread, std::ref(res[i])));
  }

  for (auto &r : thread_pool) {
    r.join();
  }
  return linear_sum(res.data(), res.size());
}


template<typename T>
std::vector<T> reduceHelper(const T  *array, std::size_t seed_size, std::size_t length) {
  std::size_t calc_per_thread = length / seed_size;
  std::vector<T> res(seed_size);
  std::vector<std::thread> thread_pool;
  for (int i = 0; i < seed_size; ++i) {
    thread_pool.emplace_back(std::thread(sum2<int>, &array[i*calc_per_thread], calc_per_thread, std::ref(res[i])));
  }
  for (auto &r : thread_pool) {
    r.join();
  }

  std::size_t dec_seed_size = seed_size / 4;
  switch (dec_seed_size) {
  case 1:
  {
    res[0] += res[1] + res[2] + res[3];
    return res;
  }break;
  case 2:
  {
    res[0] += res[1] + res[2] + res[3] + res[4] + res[5] + res[6] + res[7];
    return res;
  }
  }

  return reduceHelper(res.data(), dec_seed_size, res.size());
}


template<typename T>
T reduceSum2(const T  *data, std::size_t seed_size, std::size_t  length) {
  return reduceHelper(data, seed_size, length)[0];
}


//template<template<class T, class Alloc = std::allocator<T>> class Container = std::vector>
template<int grain, class T, class Container = std::vector<T>>
T parallel_reduce2(typename const Container::const_iterator  &begin, typename const Container::const_iterator  &end) {
  std::size_t len = end - begin;

  if (len < grain) {
    return std::accumulate(begin, end, 0);
  }

  typename Container::const_iterator mid = begin + len/2;
  auto handle = std::async(std::launch::async, parallel_reduce2<grain, T, Container>, mid, end);
  T sum = parallel_reduce2<grain, T, Container>(begin, mid);
  return sum + handle.get();
}