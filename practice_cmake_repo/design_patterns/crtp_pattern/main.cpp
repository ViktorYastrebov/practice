#include <iostream>
#include <typeinfo>
#include <typeindex>

#include <list>

#include "crpt_test.hpp"

template<class implementation>
class interface_printer {
public:
  virtual ~interface_printer() {};
  void print() {
    impl().print();
  }
private:
  implementation &impl() {
    return *static_cast<implementation*>(this);
  }
};

class implementation_printer : public interface_printer<implementation_printer>
{
public:
  virtual ~implementation_printer() {}
  void print() {
    std::cout << "some string" << std::endl;
  }
};

template<typename type>
class int_implementation_printer : public interface_printer<int_implementation_printer<type>>
{
public:
  virtual ~int_implementation_printer() {}
  void print() {
    std::cout << "type: " << typeid(type).name() << " default value: " << type() << std::endl;
  }
};


auto main() -> int {
  
  interface_printer<implementation_printer> p1;
  p1.print();
  interface_printer<int_implementation_printer<int>> p2;
  p2.print();
  return 0;
}