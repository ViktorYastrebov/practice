#pragma once

#include <memory>
#include "expr_node.h"

enum TRIG_FUNC {
  SIN = 0,
  COS,
  TAN
};

template<TRIG_FUNC func>
class trigonometric_expr : public expr_node
{
public:
  trigonometric_expr(const std::shared_ptr<expr_node>  &expr)
    : expr_node(),  expr_(expr), func_(func)
  {}

  trigonometric_expr(const std::shared_ptr<expr_node>  &expr, const std::shared_ptr<expr_node>  &power)
    :expr_node(), expr_(expr), func_(func), power_(power)
  {}

  virtual ~trigonometric_expr()
  {}

  virtual AST_NODE_TYPE  id() const {
    return TRIGONOMETRIC_FUNC;
  }

  TRIG_FUNC get_func() const {
    return func_;
  }

  std::shared_ptr<expr_node>  &expr() const {
    return expr_;
  }

  std::shared_ptr<expr_node> power() const {
    return power_;
  }


  std::shared_ptr<expr_node>  &expr() {
    return expr_;
  }
  LOKI_DEFINE_VISITABLE();

private:
  std::shared_ptr<expr_node> expr_;
  TRIG_FUNC func_;
  std::shared_ptr<expr_node> power_;
};
