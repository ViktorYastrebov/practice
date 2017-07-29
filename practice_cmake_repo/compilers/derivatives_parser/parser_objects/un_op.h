#pragma once

#include "expr_node.h"
#include <memory>
#include <string>

class unary_op : public expr_node {
public:
  enum SING_TYPE {
    BASE = 0, PLUS, MINUS
  };
  unary_op();
  unary_op(const std::shared_ptr<expr_node>  &expr, SING_TYPE op);
  virtual ~unary_op();
  virtual AST_NODE_TYPE id() const;
  std::shared_ptr<expr_node>  expr() const;
  SING_TYPE sign_type() const;
  std::string sign_str() const;
  LOKI_DEFINE_VISITABLE();
private:
  std::shared_ptr<expr_node> expr_;
  SING_TYPE op_;
};