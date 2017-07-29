#pragma once

#include "base_ast_node.h"

class expr_node : public base_ast_node {
public:
  expr_node();
  virtual ~expr_node();
  virtual AST_NODE_TYPE  id() const;
  virtual bool isConstant() const;
  LOKI_DEFINE_VISITABLE();
};