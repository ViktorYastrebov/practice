#pragma once

#include <type_traits>

template<typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
void doStuff(const T  &val) {
  std::cout << "intergal type, " << val << std::endl;
}

template<typename T, typename std::enable_if<std::is_same<std::string, T>::value>::type* = nullptr>
void doStuff(const T  &val) {
  std::cout << "std::string type, " << val << std::endl;
}

// #4, enabled via a template parameter
template<class T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
void print(const T  &value)
{
  std::cout << "std::is_integral, value = " << value << std::endl;
}

// #5, enabled via a template parameter
template<class T, typename std::enable_if<std::is_same<std::string, T>::value>::type* = nullptr>
void print(const T  &value) // note, function signature is unmodified
{
  std::cout << "std::is_same<T, std::string>, value = " << value << std::endl;
}