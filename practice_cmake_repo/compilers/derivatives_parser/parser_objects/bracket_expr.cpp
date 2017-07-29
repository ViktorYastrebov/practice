#include "bracket_expr.h"


bracket_expr::bracket_expr()
  :expr_node()
{}

bracket_expr::bracket_expr(const std::shared_ptr<expr_node> &expr)
  :expr_node(), expr_(expr)
{}

bracket_expr::~bracket_expr()
{}

std::shared_ptr<expr_node> bracket_expr::expr() const {
  return expr_;
}

base_ast_node::AST_NODE_TYPE  bracket_expr::id() const {
  return BRACKETED_EXPR;
}