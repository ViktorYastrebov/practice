#include <iostream>
#include <functional>
#include <string>

#include <type_traits>
//http://en.cppreference.com/w/cpp/utility/functional/invoke

struct S {
  template<class T, typename std::enable_if<std::is_same<T, int>::value>::type* = nullptr>
  void f(const T  &value) {
    std::cout << value << std::endl;
  }
};

void function_args(int a, const char  *b, const double  *c) {
  std::cout << "int = " << a << ", literal = " << b << ", double = " << *c << std::endl;
}

int main(int argc, char*argv[]) {
  
  S s;
  {
    std::function<void(int)> f = std::bind(&S::f<int>, &s, std::placeholders::_1);
    f(38);
  }
  s.f<int>(37);

  double value = 3.14;
  std::function<void(int a, const char*, double *)> complex2 = std::bind(function_args, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
  std::function<void()> complex = std::bind(function_args, 1, "literal", &value);
  complex();
  complex2(1, "string", &value);
  return 0;
}