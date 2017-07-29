#include <iostream>
#include "parallel_reduce.hpp"

auto main() -> int {

  const std::size_t arr_length = 1'048'576*64;
  std::vector<int> arr_data(arr_length, 1);
  /*
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, 5);

  for (int i = 0; i < arr_length; ++i) {
    arr_data[i] = dis(gen);
  }*/

  // 100'000'000

  std::cout << "max parallel threads = " << std::thread::hardware_concurrency() << std::endl;

  auto start = std::chrono::high_resolution_clock::now();
  int value = linear_sum(arr_data.data(), arr_length);
  auto end = std::chrono::high_resolution_clock::now();
  auto diff = end - start;
  std::cout << "value = " << value << std::endl;
  std::cout <<"linear sum time duration = "  << std::chrono::duration<double, std::milli>(diff).count() << std::endl;
/*
  auto start = std::chrono::high_resolution_clock::now();
  auto value = reduceSum(arr_data.data(), std::thread::hardware_concurrency() * 2, arr_length);
  auto end = std::chrono::high_resolution_clock::now();
  auto diff = end - start;
  std::cout << "value = " << value << std::endl;
  std::cout << "parallel sum reduction time duration =" << std::chrono::duration<double, std::milli>(diff).count() << std::endl;
*/
  // http://stackoverflow.com/questions/21937627/why-parallel-accumulate-is-slower
  start = std::chrono::high_resolution_clock::now();
  //value = reduceSum(arr_data.data(), std::thread::hardware_concurrency() * 2, arr_length);
  value = reduceSum2(arr_data.data(), std::thread::hardware_concurrency() * 2, arr_length);
  end = std::chrono::high_resolution_clock::now();
  diff = end - start;
  std::cout << "value = " << value << std::endl;
  std::cout << "parallel sum reduction2 time duration =" << std::chrono::duration<double, std::milli>(diff).count() << std::endl;


  // statistic table:
  //  [1.5, 1.48, 1.48, 0.85, 1.7]

  start = std::chrono::high_resolution_clock::now();
  //int sum = parallel_reduce2<524'288, int>(arr_data.begin(), arr_data.end());
  int sum = parallel_reduce2<524'288, int>( arr_data.begin(), arr_data.end());
  end = std::chrono::high_resolution_clock::now();
  diff = end - start;
  std::cout << "value = " << value << std::endl;
  std::cout << "parallel_reduce2 time duration =" << std::chrono::duration<double, std::milli>(diff).count() << std::endl;
  return 0;
}