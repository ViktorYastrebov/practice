#ifndef RTTYPE_STACK_H
#define RTTYPE_STACK_H

#include "RTTypeCommon.h"

#include <loki/Pimpl.h>


class RTTypeStack {
public:
  RTTypeStack();

  ~RTTypeStack();

  void pushItem(const RTType& v);

  void popItem();

  RTType topItem() const;

  int getSize() const;

  bool isEmpty() const;

private:
  Loki::PimplOf<RTTypeStack>::Type impl_;
};


#endif /*RTTYPE_STACK_H*/
