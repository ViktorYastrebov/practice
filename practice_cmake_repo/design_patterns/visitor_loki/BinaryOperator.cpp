#include "BinaryOperator.h"


BinaryOperator::BinaryOperator(Expression  *left, Expression  *right, BINARY_OPERATOR_TYPES op)
  :Expression(), m_left(left), m_right(right), m_op(op)
{}

BinaryOperator::~BinaryOperator() {
  delete m_left;
  delete m_right;
}

const Expression  *BinaryOperator::getLeft() const {
  return m_left;
}

Expression  *BinaryOperator::getLeft() {
  return m_left;
}

const Expression  *BinaryOperator::getRight() const {
  return m_right;
}

Expression  *BinaryOperator::getRight() {
  return m_right;
}

BinaryOperator::BINARY_OPERATOR_TYPES BinaryOperator::getOperation() const {
  return m_op;
}