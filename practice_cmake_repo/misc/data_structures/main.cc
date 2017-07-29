/*
#include <iostream>

#include "stack.hpp"

int main() {

  practice::Stack<int> s;

  s.push(1);
  s.pop();

  return 0;
}*/

#include <iostream>
/*
class A
{
public:
    A(int n = 0)
        : m_n(n)
    {
        std::cout << 'd';
    }

    A(const A& a)
        : m_n(a.m_n)
    {
        std::cout << 'c';
    }

private:
    int m_n;
};

void f(const A &a1, const A &a2 = A()) {}

void test1() {
  A a(2), b;
  const A c(a);
  const A & d = c;
  const A e = b;
  b = d;
  A *p = new A(c), *q = &a;
  static_cast<void>(q);
  delete p;
  f(3);
  std::cout << std::endl;
}*/

class A
{
public:
    A(int n = 2) : m_i(n) { }

    ~A() { std::cout << m_i; }

protected:
    int m_i;
};

class B
    : public A
{
public:
    B(int n) : m_x(m_i + 1) , m_a(n) { }

public:
    ~B()
    {
        std::cout << m_i;
        --m_i;
    }

private:
    A m_x;
    A m_a;
};

int main()
{
    { B b(5); }

    std::cout << std::endl;

    return 0;
}