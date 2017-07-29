#pragma once 
#include <type_traits>
#include <string>
#include "funcions.hpp"

struct doStuffStuct
{
  /*
  template<class T, typename Enabled = void>
  void print(const T  &value) {
  std::cout << "default handler";
  } */
  template<class T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
  void print(const T  &value) {
    std::cout << "doStuff::print::integral type";
  }
  template<class T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
  void print(const T  &value) {
    std::cout << "doStuff::print::std::string type";
  }
};

//http://stackoverflow.com/questions/13401716/selecting-a-member-function-using-different-enable-if-conditions
//http://en.cppreference.com/w/cpp/types/enable_if
//conclucion - make the template specification directly. 
// do not make the another cases and specified

void applying_type_slicing() {
  std::string value("some value");
  print<std::string>(value);
  doStuff<std::string>(value);

  doStuffStuct st;
  st.print<std::string>(value);
  //st.print<doStuffStuct>(st);
}

#include <array>
#include <iostream>
/*
template<class T, int Type, std::size_t size = 0, typename EnT = void>
struct base_a {
  void test() {
    std::cout << "something";
  }
};

template<class T, int Type, std::size_t size>
struct base_a<T,Type, size, typename std::enable_if<std::is_same<T, char>::value>::type*>
{
  void test() {
    std::cout << "array type";
  }
  std::array<T, size> value_;
};

template<class T, int Type>
struct base_a<T, Type, 0> {
  void test() {
    std::cout << "simple type";
  }
  T value_;
};

void test_base() {
  base_a<int, 10> a;
  a.test();
  base_a<char, 10, 10> b;
  b.test();
  base_a<int, 10, 2> c; //there must be an compiletime error
  c.test();
}*/

#if 0
template<class A, class B, class EnT = void>
struct enableTypeTest
{
  static_assert(!std::is_same<A, char>::value, "there is only allowed char array type");
};

template<class A, class B>
struct enableTypeTest<A, B, typename std::enable_if<std::is_same<A, char>::value>::type >
{
  A value_;
};

void test_base() {
  enableTypeTest<int, float> a;
  enableTypeTest<char, float> b;
}
#endif

template<class A, int size = 0, class EnT = void>
struct enableTypeTest
{
  A value_ = A();
};

template<class A, int size>
struct enableTypeTest<A, size, typename std::enable_if<std::is_same<A, char>::value>::type >
{
  static_assert(size > 0, " size must be greater 0");
  std::array<A, size> value_{};
};

void test_base() {
  enableTypeTest<int, 1000> a; //there should be compiletime error
  std::cout << a.value_;
  enableTypeTest<char, 10> b;
  std::cout << b.value_[0];
  enableTypeTest<int> c;
  std::cout << c.value_;

  enableTypeTest<char> d;
  std::cout << d.value_[0];
}
