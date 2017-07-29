#pragma once
#include <typeindex>

#include "expr_node.h"


class constant_base : public expr_node {
public:
  constant_base()
    :expr_node()
  {}

  virtual ~constant_base()
  {}

  virtual AST_NODE_TYPE id() const
  {
    return CONSTANT;
  }

  virtual bool isConstant() const {
    return true;
  }
  virtual std::type_index type() const = 0;
};

template<class T>
class constant : public constant_base {
public:
  constant()
    : constant_base(), value_()
  {}

  constant(const T  &value)
    :value_(value)
  {}

  virtual ~constant()
  {}

  T& value() {
    return value_;
  }

  virtual std::type_index type() const {
    return typeid(T);
  }

  const T& value() const {
    return value_;
  }
  LOKI_DEFINE_VISITABLE();
private:
  T value_;
};
