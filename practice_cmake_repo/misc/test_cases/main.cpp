#include <iostream>
#include <string>

template<typename T>
struct linked_list {
  T value_;
  linked_list<T>   *next_;
};

template<typename T>
bool hasLoop(const linked_list<T>  &list) {
  linked_list<T>   *current = list.next_;
  linked_list<T>   *second = list.next_->next_;
  while (second->next_ != nullptr && second != nullptr && current != nullptr) {
    if (current == second) {
      return true;
    }
    current = current->next_;
    second = second->next_->next_;
  }
  return false;
}

//fn:  0 1 1 2 3 5 8 13 2
//nth: 0 1 2 3 4 5 6 7
// fn = fn-1 + fn-2

int fibonachi(int nth) {
  if (nth < 2) {
    return nth;
  }
  return fibonachi(nth - 1) + fibonachi(nth - 2);
}

template<int value>
struct Fibonachi {
  static const int value = Fibonachi<value - 1>::value + Fibonachi<value - 2>::value;
};

template<>
struct Fibonachi<0> {
  static const int value = 0;
};

template<>
struct Fibonachi<1> {
  static const int value = 1;
};

void revert_string(char  *str) {
  if (!str || !*str || !str[1]) {
    return;
  }
}

std::string reverse_string(const std::string  &str) {
  if (str.empty()) {
    return std::string();
  }
  std::string::value_type head = str.at(0);
  return reverse_string(str.substr(1, str.length())) + head;
}


class Base {
public:
  virtual bool isTrue() const = 0;
  virtual ~Base() = 0 {};
};

class Derived : public Base {
public:
  virtual ~Derived() {
  }
  bool isTrue() const override {
    return 0;
  }
};

auto main() -> int {

  linked_list<int> a, b, c, d, e;
  a.next_ = &b;
  b.next_ = &c;
  c.next_ = &d;
  d.next_ = &e;
  e.next_ = &a;
  
  std::cout << hasLoop(a) << std::endl;
  
  std::cout << fibonachi(7) << std::endl;
  std::cout << Fibonachi<7>::value << std::endl;
  std::cout << reverse_string("1234567") << std::endl;

  Base  &ref = Derived();
  ref.isTrue();

}