#ifndef QUEUE_H
#define QUEUE_H

#include "Container.hpp"

namespace practice {

template<class T, class container = Container<T> >
class Queue {
public:
  Queue();
  Queue(const size_t size);
  ~Queue();

  void push(const T & rvalue);
  void pop();

  const T & front() const;
  T & front();

  const T & back() const;
  T & back();

  const container & getContainer() const;
  container & getContainer();

protected:
  container * m_head;
  container * m_tails;
  size_t m_size;
};

template<class T, class container = Container<T>>
Queue<T>::Queue()
  :m_size(0)
{
  m_head = m_tails = 0;
}

template<class T, class container = Container<T>>
Queue<T>::Queue(const size_t size)
  :m_size(size)
{
  if(size) {

    m_head = new Container();
    m_tails = m_head;

    for(size_t i = 1; i < m_size; ++i) {
      
    }
  } else {
    m_head = m_tails = 0;
  }
}

}
#endif