#pragma once

#include "Expression.h"

class BinaryOperator : public Expression {
public:
  enum BINARY_OPERATOR_TYPES {
    OP_EQL,
    OP_NEQ,
  };

  BinaryOperator(Expression  *left, Expression  *right, BINARY_OPERATOR_TYPES op);
  ~BinaryOperator();

  LOKI_DEFINE_VISITABLE();

  const Expression  *getLeft() const;
  Expression  *getLeft();

  const Expression  *getRight() const;
  Expression  *getRight();

  BINARY_OPERATOR_TYPES getOperation() const;

private:
  BINARY_OPERATOR_TYPES m_op;
  Expression  *m_left;
  Expression  *m_right;
};
