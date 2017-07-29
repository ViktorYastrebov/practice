#pragma once

#include <loki/Visitor.h>

#include "../lexer/tokens.hpp"

class base_ast_node : public Loki::BaseVisitable<void, Loki::DefaultCatchAll> {
public:

  enum AST_NODE_TYPE{
    BASE_AST_NODE = 0,
    EXPR_AST_NODE,
    BRACKETED_EXPR,
    UN_OP,
    BIN_OP,
    IDENT,
    CONSTANT,
    CONTANT_VARIABLE,
    TRIGONOMETRIC_FUNC
  };

  base_ast_node();
  virtual ~base_ast_node();
  virtual AST_NODE_TYPE  id() const = 0;
  LOKI_DEFINE_VISITABLE();
};