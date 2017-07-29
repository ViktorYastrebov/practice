#include "un_op.h"

unary_op::unary_op()
  :expr_node(), op_(BASE)
{}

unary_op::unary_op(const std::shared_ptr<expr_node>  &expr, SING_TYPE op)
  :expr_(expr), op_(op)
{}

unary_op::~unary_op()
{}

unary_op::AST_NODE_TYPE unary_op::id() const {
  return UN_OP;
}

std::shared_ptr<expr_node>  unary_op::expr() const {
  return expr_;
}

unary_op::SING_TYPE unary_op::sign_type() const {
  return op_;
}

std::string unary_op::sign_str() const {
  static char *values[] = { "base", "plus", "minus" };
  return std::string(values[op_]);
}