#include "expr_node.h"

expr_node::expr_node()
  :base_ast_node()
{}

expr_node::~expr_node()
{}

expr_node::AST_NODE_TYPE  expr_node::id() const {
  return EXPR_AST_NODE;
}

bool expr_node::isConstant() const {
  return false;
}