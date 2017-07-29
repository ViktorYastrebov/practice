#pragma once

#include <memory>
#include "expr_node.h"

class bracket_expr : public expr_node {
public:
  bracket_expr();
  virtual ~bracket_expr();
  bracket_expr(const std::shared_ptr<expr_node> &expr);
  virtual AST_NODE_TYPE  id() const;

  std::shared_ptr<expr_node> expr() const;
  LOKI_DEFINE_VISITABLE();
private:
  std::shared_ptr<expr_node> expr_;
};