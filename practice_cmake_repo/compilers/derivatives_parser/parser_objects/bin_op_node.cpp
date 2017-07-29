#include "bin_op_node.h"

bin_op_node::bin_op_node()
  :expr_node(), op_(NO_OP)
{}

bin_op_node::bin_op_node(const std::shared_ptr<expr_node>  &left, BINOP_TYPE op, const std::shared_ptr<expr_node>  &right)
  :expr_node(), left_(left), op_(op), right_(right)
{}

bin_op_node::~bin_op_node()
{}

bin_op_node::AST_NODE_TYPE bin_op_node::id() const {
  return BIN_OP;
}


std::shared_ptr<expr_node> bin_op_node::get_left() const {
  return left_;
}

std::shared_ptr<expr_node> bin_op_node::get_right() const {
  return right_;
}

bin_op_node::BINOP_TYPE bin_op_node::get_op() const {
  return op_;
}

std::string bin_op_node::str_op() const {
  static char  *values[] = { "no_op", "^", "*", "/", "+", "-"};
  return std::string(values[op_]);
}

std::wstring bin_op_node::wstr_op() const {
  static wchar_t  *values[] = { L"no_op", L"^", L"*", L"/", L"+", L"-" };
  return std::wstring(values[op_]);
}
