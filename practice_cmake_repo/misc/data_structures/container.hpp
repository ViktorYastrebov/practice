#ifndef CONTAINER_H
#define CONTAINER_H

namespace practice {

template<class T> 
class Container {
public:
  Container();
  Container(const T & rvalue);
  ~Container();

  Container<T> & operator = (const Container<T> & rvalue);

  const T & get() const;
  T & get();

private:
  T m_data;
};

template<class T> 
Container<T>::Container()
  :m_data(T())
{}

template<class T> 
Container<T>::~Container()
{}

template<class T>
Container<T>::Container(const T & rvalue) {
  m_data = rvalue;
}

template<class T>
Container<T> & Container<T>::operator = (const Container<T> & rvalue) {
  m_data = rvalue.get();
  return *this;
}

template<class T>
const T & Container<T>::get() const {
  return m_data;
}

template<class T>
T & Container<T>::get() {
  return m_data;
}

}
#endif