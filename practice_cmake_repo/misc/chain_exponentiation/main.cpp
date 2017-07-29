#include <iostream>
#include "chain_exp.hpp"

template<class T>
void testValidate(T val, int exp) {
  int steps = 0;

  std::cout << "value =" << val << "exp = " << exp << std::endl;
  T value = chainExponet<int>(exp, val, steps);
  std::cout << "result = " << value << " steps = " << steps << std::endl;
}


int main(int argc, char*argv[]) {

  testValidate<int>(2, 3);
  testValidate<int>(3, 4);
  testValidate<int>(2, 15);

  return 0;
}