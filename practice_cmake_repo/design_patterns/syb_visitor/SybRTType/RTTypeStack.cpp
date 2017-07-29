#include "RTTypeStack.h"

#include "rttype/rttype.h"

#include <stack>


template<>
struct Loki::ImplOf<RTTypeStack> {
  ImplOf() {}
  ~ImplOf() {}

  std::stack<RTType> rttypeObjects;
};


RTTypeStack::RTTypeStack() {}


RTTypeStack::~RTTypeStack() {}


void RTTypeStack::pushItem(const RTType& v) {
  impl_->rttypeObjects.push(v);
}


void RTTypeStack::popItem() {
  impl_->rttypeObjects.pop();
}


RTType RTTypeStack::topItem() const {
  return impl_->rttypeObjects.top();
}


bool RTTypeStack::isEmpty() const {
  return impl_->rttypeObjects.empty();
}


int RTTypeStack::getSize() const {
  return impl_->rttypeObjects.size();
}
