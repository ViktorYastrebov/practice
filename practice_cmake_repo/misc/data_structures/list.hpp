#ifndef LIST_H_
#define LIST_H

namespace practice {

template<class T>
struct Node {
public:
  T m_data;
  Node<T> * m_next;
  Node<T> * m_prev;
};

template<class T>
class ListVal {
public:
  ListVal();
  ~ListVal();
  ListVal(const ListVal<T> & rvalue);
  ListVal<T> & operator = (const ListVal<T> & rvalue);

  const T & front() const;
  T & front();

  const T & back() const;
  T & back();

  void push_front(const T & val);
  void pop_front();

  void push_back(const T & val);
  void pop_back();

  bool empty() const;
  int size() const;
  
  void clear();

  Node<T> * m_head;
  Node<T> * m_tail;
private:
  int m_size;
};

template<class T>
ListVal<T>::ListVal()
  :m_size(0), m_head(0),m_tail(0) 
{}

template<class T>
ListVal<T>::~ListVal() {
  clear();
}

template<class T>
ListVal<T>::ListVal(const ListVal<T> & rvalue) {

  if(rvalue != *this) {
    clear();

    if(!rvalue.empty) {
      m_head = new Node<T>;
      m_head->m_data = rvalue.m_head->m_data;
      m_tail = m_head->m_next;
      Node<T> * prev = m_head;

      for(Node<T > * iter = rvalue.m_head->m_next; iter != rvalue.m_tail->m_next; iter = iter->m_next) {
        m_tail = new Node<T>;
        m_tail->m_data = iter->m_data;
        m_tail->m_prev = prev;
        prev = m_tail;
        m_tail = m_tail->m_next;
      }
      m_size = rvalue.size;
    }
  }
}

template<class T>
ListVal<T> & ListVal<T>::operator = (const ListVal<T> & rvalue) {
  if(rvalue != *this) {
    clear();

    if(!rvalue.empty) {
      m_head = new Node<T>;
      m_head->m_data = rvalue.m_head->m_data;
      m_tail = m_head->m_next;
      Node<T> * prev = m_head;

      for(Node<T > * iter = rvalue.m_head->m_next; iter != rvalue.m_tail->m_next; iter = iter->m_next) {
        m_tail = new Node<T>;
        m_tail->m_data = iter->m_data;
        m_tail->m_prev = prev;
        prev = m_tail;
        m_tail = m_tail->m_next;
      }
      m_size = rvalue.size;
    }
  }
  return *this;
}

template<class T>
const T & ListVal<T>::front() const {
  return m_head->m_data;
}

template<class T>
T & ListVal<T>::front() {
  return m_head->m_data;
}

template<class T>
const T & ListVal<T>::back() const {
  return m_tail->m_data;
}

template<class T>
T & ListVal<T>::back() {
  return m_tail->m_data;
}

template<class T>
void ListVal<T>::push_front(const T & val) {
  Node<T> * add  = new Node<T>;
  Node<T> * cur_head = m_head;

  add->m_data = val;
  add->m_prev = 0;
  m_head = add;

  if(!m_size) {
    m_tail = m_head;
    m_tail->m_next = 0;
    ++m_size;
    return;
  }

  if(m_size == 1) {
    add->m_next = cur_head;
    cur_head->m_prev = m_head;
    ++m_size;
    return;
  }

  m_head->m_next = cur_head;
  cur_head->m_prev = m_head;

  ++m_size;
}

template<class T>
void ListVal<T>::pop_front() {
  Node<T> * del = m_head;
  m_head = m_head->m_next;
  delete del;

  --m_size;
}

template<class T>
void ListVal<T>::push_back(const T & val) {
  Node<T> * add = new Node<T>;
  Node<T> * cur_tail = m_tail;

  add->m_data = val;
  add->m_next = 0;
  m_tail = add;

  if(!m_size) {
    m_head = m_tail;
    m_head->m_prev = 0;
    ++m_size;
    return;
  }

  if(m_size == 1) {
    add->m_prev = cur_tail;
    cur_tail->m_next = m_tail;
    ++m_size;
    return;
  }

  m_tail->m_prev = cur_tail;
  cur_tail->m_next = m_tail;
  ++m_size;
}

template<class T>
void ListVal<T>::pop_back() {
  Node<T> * del = m_tail;

  m_tail = m_tail->m_prev;
  
  delete del;
  --m_size;
  if(!m_size) {
    m_head = 0;
  }
}

template<class T>
bool ListVal<T>::empty() const {
  return m_size == 0;
}

template<class T>
int ListVal<T>::size() const {
  return m_size;
}

template<class T>
void ListVal<T>::clear() {

  while(m_head) {
    Node<T> * del = m_head;
    m_head = m_head->m_next;
    delete del;
  }

  m_head = 0;
  m_tail = 0;
  m_size = 0;
}

template<class T, template<typename> class NodeType>
class ListIterator {
public:

  ListIterator();
  ListIterator(NodeType<T> *ptr);

  T & operator *() const;
  ListIterator<T, NodeType > & operator++();
  ListIterator<T, NodeType > operator++(int);
  ListIterator<T, NodeType > & operator--();
  ListIterator<T, NodeType >  operator--(int);

  NodeType<T> * _getPtr() const {
    return m_ptr;
  }
  
  bool operator == (const ListIterator & rvalue) {
    return (this->m_ptr == rvalue._getPtr());
  }

  bool operator != (const ListIterator & rvalue) {
    return !(*this == rvalue);
  }

private:
  NodeType<T> * m_ptr;
};

template<class T, template<typename> class NodeType>
ListIterator<T, NodeType >::ListIterator()
  :m_ptr(0)
{}

template<class T, template<typename> class NodeType>
ListIterator<T, NodeType >::ListIterator(NodeType<T> *ptr) 
  :m_ptr(ptr)
{}

template<class T, template<typename> class NodeType>
T & ListIterator<T, NodeType >::operator*() const {
  return m_ptr->m_data;
}

template<class T, template<typename> class NodeType>
ListIterator<T, NodeType > & ListIterator<T, NodeType >::operator++() {
  m_ptr = m_ptr->m_next;
  return *this;
}

template<class T, template<typename> class NodeType>
ListIterator<T, NodeType> ListIterator<T, NodeType>::operator++(int) {
  ListIterator<T, NodeType<T>> tmp = *this;
  ++*this;
  return tmp;
}

template<class T, template<typename> class NodeType>
ListIterator<T, NodeType> & ListIterator<T, NodeType>::operator--() {
  m_ptr = m_ptr->m_prev;
  return *this;
}

template<class T, template<typename> class NodeType>
ListIterator<T, NodeType> ListIterator<T, NodeType>::operator--(int) {
  ListIterator<T, NodeType<T>> tmp = *this;
  --*this;
  return tmp;
}

template<class T, template<typename> class NodeType>
class ListConstInterator {
public:
  ListConstInterator()
    :m_ptr(0)
  {}

  ListConstInterator(const ListIterator<T,NodeType> &x) 
    : m_ptr(x._getPtr())
  {}

  ListConstInterator(NodeType<T> *ptr)
    :m_ptr(ptr)
  {}

  const T & operator *() const {
    return m_ptr->m_data;
  }

  ListConstInterator<T, NodeType > & operator++() {
    m_ptr = m_ptr->m_next;
    return *this;
  }
  
  ListConstInterator<T, NodeType > operator++(int) {
    ListConstInterator<T, NodeType<T>> tmp = *this;
    ++*this;
    return tmp;
  }

  ListConstInterator<T, NodeType > & operator--() {
    m_ptr = m_ptr->m_prev;
    return *this;
  }

  ListConstInterator<T, NodeType >  operator--(int) {
    ListConstInterator<T, NodeType<T>> tmp = *this;
    --*this;
    return tmp;
  }

  NodeType<T> * _getPtr() const {
    return m_ptr;
  }

  bool operator == (const ListConstInterator & rvalue) {
    return (this->m_ptr == rvalue._getPtr());
  }

  bool operator != (const ListConstInterator & rvalue) {
    return !(*this == rvalue);
  }

private:
  NodeType<T> * m_ptr;
};

template<class T>
class List {
public:
  typedef ListConstInterator<T, Node> const_iterator;
  typedef ListIterator<T, Node> iterator;

  List();
  ~List();
  List(const List<T> & rvalue);

  List<T> & operator = (const List<T> & rvalue);

  const T & front() const;
  T & front();

  const T & back() const;
  T & back();

  void push_front(const T & val);
  void pop_front();

  void push_back(const T & val);
  void pop_back();

  bool empty() const;
  int size() const;

  void clear();

  iterator begin() {
    iterator tmp(m_list.m_head);
    return tmp;
  }

  const_iterator begin() const {
    const_iterator tmp(m_list.m_head);
    return tmp;
  }

  iterator end() {
    iterator tmp(m_list.m_tail->m_next);
    return tmp;
  }

  const_iterator end() const {
    const_iterator tmp(m_list.m_tail->m_next);
    return tmp;
  }

  iterator rbegin() {
    iterator tmp(m_list.m_tail);
    return tmp;
  }

  const_iterator rbegin() const {
    const_iterator tmp(m_list.m_tail);
    return tmp;
  }

  iterator rend() {
    iterator tmp(m_list.m_head->m_prev);
    return tmp;
  }

  const_iterator rend() const {
    const_iterator tmp(m_list.m_head->m_prev);
    return tmp;
  }

private:
  ListVal<T> m_list;
};

template<class T>
List<T>::List()
  :m_list()
{}

template<class T>
List<T>::~List() {
}

template<class T> 
List<T>::List(const List<T> & rvalue) {
  
  clear();
  for(List<T>::const_iterator i = rvalue.begin(); i != rvalue.end(); ++i) {
    m_list.push_back(*i);
  }
}

template<class T> 
List<T> & List<T>::operator = (const List<T> & rvalue) {
  clear();
  for(List<T>::const_iterator i = rvalue.begin(); i != rvalue.end(); ++i) {
    m_list.push_back(*i);
  }
  return *this;
}

template<class T>
const T & List<T>::front() const {
  return m_list.front();
}

template<class T>
T & List<T>::front() {
  return m_list.front();
}

template<class T>
const T & List<T>::back() const {
   return m_list.back();
}

template<class T>
T & List<T>::back() {
  return m_list.back();
}

template<class T>
void List<T>::push_front(const T & val) {
  m_list.push_front(val);
}

template<class T>
void List<T>::pop_front() {
  m_list.pop_front();
}

template<class T>
void List<T>::push_back(const T & val) {
  m_list.push_back(val);
}

template<class T>
void List<T>::pop_back() {
  m_list.pop_back();
}

template<class T>
bool List<T>::empty() const {
  return m_list.empty();
}

template<class T>
int List<T>::size() const {
  return m_list.size();
}

template<class T>
void List<T>::clear() {
  m_list.clear();
}

}
#endif