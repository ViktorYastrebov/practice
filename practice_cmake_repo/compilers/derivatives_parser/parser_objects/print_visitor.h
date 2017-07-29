#pragma once

#include "common_incl.hpp"
#include <loki/Visitor.h>

class print_visitor
  : public Loki::BaseVisitor,
  public Loki::Visitor<base_ast_node>,
  public Loki::Visitor<expr_node>,
  public Loki::Visitor<bracket_expr>,
  public Loki::Visitor<constant<unsigned int>>,
  public Loki::Visitor<constant<double>>,
  public Loki::Visitor<identifier>,
  public Loki::Visitor<constant_variable>,
  public Loki::Visitor<unary_op>,
  public Loki::Visitor<bin_op_node>,
  public Loki::Visitor<trigonometric_expr<TRIG_FUNC::COS>>,
  public Loki::Visitor<trigonometric_expr<TRIG_FUNC::SIN>>,
  public Loki::Visitor<trigonometric_expr<TRIG_FUNC::TAN>>
{
public:
  print_visitor();
  virtual ~print_visitor();

  void Visit(base_ast_node  &base);
  void Visit(expr_node  &expr);
  void Visit(bracket_expr &br_expr);
  void Visit(constant<unsigned int>  &cnst);
  void Visit(constant<double>  &cnst);
  void Visit(identifier  &ident);
  void Visit(constant_variable  &var);
  void Visit(unary_op  &un);
  void Visit(bin_op_node  &bin);
  void Visit(trigonometric_expr<TRIG_FUNC::COS>  &cos);
  void Visit(trigonometric_expr<TRIG_FUNC::SIN>  &sin);
  void Visit(trigonometric_expr<TRIG_FUNC::TAN>  &tan);
};
