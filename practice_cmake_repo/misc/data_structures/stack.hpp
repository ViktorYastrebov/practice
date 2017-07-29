#ifndef STACK_H
#define STACK_H

#include "list.hpp"

namespace practice {

template<class T>
class Stack {
public:
  Stack();
  ~Stack();

  void push(const T & value);
  void pop();

  const T & top() const;
  T & top();

  bool empty() const;
  int size() const;

private:
  List<T> m_stack;
};

template<class T>
Stack<T>::Stack()
{}

template<class T>
Stack<T>::~Stack()
{}

template<class T>
void  Stack<T>::push(const T & value) {
  m_stack.push_back(value);
}

template<class T>
void Stack<T>::pop() {
  m_stack.pop_back();
}

template<class T>
const T & Stack<T>::top() const {
  return m_stack.back();
}

template<class T>
T & Stack<T>::top() {
  return m_stack.back();
}

template<class T>
bool Stack<T>::empty() const {
  return m_stack.empty();
}

template<class T>
int Stack<T>::size() const {
  return m_stack.size();
}

}

#endif