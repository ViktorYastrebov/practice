#pragma once

#include "expr_node.h"

#include <memory>
#include<string>

/*
class base_op_strategy
{
public: 
  virtual ~base_op_strategy() {};
};

class power_strategy : public base_op_strategy { virtual ~power_strategy() {} };
class mul_strategy : public base_op_strategy { virtual ~mul_strategy() {} };
class div_strategy : public base_op_strategy { virtual ~div_strategy() {} };
class add_strategy : public base_op_strategy { virtual ~add_strategy() {} };
*/

/*
class base_strategy {
public:
  virtual ~base_strategy();
};*/


class bin_op_node : public expr_node {
public:
  enum BINOP_TYPE {
    NO_OP = 0,
    POWER,
    MUL,
    DIV,
    PLUS,
    MINUS
  };
  //bin_op_node(const std::shared_ptr<expr_node>  &left, const std::shared_ptr<base_op_strategy>  &op, const std::shared_ptr<expr_node>  &right);
  bin_op_node();
  bin_op_node(const std::shared_ptr<expr_node>  &left, BINOP_TYPE op, const std::shared_ptr<expr_node>  &right);
  virtual ~bin_op_node();
  virtual AST_NODE_TYPE id() const;

  std::shared_ptr<expr_node> get_left() const;
  std::shared_ptr<expr_node> get_right() const;
  //std::shared_ptr<base_op_strategy>  get_op() const;
  BINOP_TYPE get_op() const;
  std::string str_op() const;
  std::wstring wstr_op() const;
  LOKI_DEFINE_VISITABLE();
private:
  std::shared_ptr<expr_node> left_;
  std::shared_ptr<expr_node> right_;
  //std::shared_ptr<base_op_strategy> op_;
  BINOP_TYPE op_;
};