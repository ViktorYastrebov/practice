#include <iostream>
#include "print_visitor.h"

print_visitor::print_visitor()
{}

print_visitor::~print_visitor()
{}

void print_visitor::Visit(base_ast_node  &base) {
  //throw std::runtime_error("logical error");
  base.Accept(*this);
}

void print_visitor::Visit( expr_node  &expr) {
  //throw std::runtime_error("logical error");
  expr.Accept(*this);
}

void print_visitor::Visit(bracket_expr &br_expr) {
  std::cout << "(";
  br_expr.expr()->Accept(*this);
  std::cout << ")";
}

void print_visitor::Visit(constant<unsigned int>  &cnst) {
  std::cout << cnst.value();
}

void print_visitor::Visit(constant<double>  &cnst) {
  std::cout << cnst.value();
}

void print_visitor::Visit(identifier  &ident) {
  std::wcout << ident.get();
}

void print_visitor::Visit(constant_variable  &var) {
  std::wcout << var.get();
}

void print_visitor::Visit(unary_op  &un) {
  std::cout << un.sign_str();
  un.expr()->Accept(*this);
}

void print_visitor::Visit(bin_op_node  &bin) {
  bin.get_left()->Accept(*this);
  std::cout << bin.str_op();
  bin.get_right()->Accept(*this);
}

void print_visitor::Visit(trigonometric_expr<TRIG_FUNC::COS>  &cos) {
  std::cout << "cos";
  if (cos.power()) {
    std::cout << "^";
    cos.power()->Accept(*this);
  }
  std::cout << "(";
  cos.expr()->Accept(*this);
  std::cout << ")";
}

void print_visitor::Visit(trigonometric_expr<TRIG_FUNC::SIN>  &sin) {
  std::cout << "sin";
  if (sin.power()) {
    std::cout << "^";
    sin.power()->Accept(*this);
  }
  std::cout << "(";
  sin.expr()->Accept(*this);
  std::cout << ")";
}

void print_visitor::Visit(trigonometric_expr<TRIG_FUNC::TAN>  &tan) {
  std::cout << "tan";
  if (tan.power()) {
    std::cout << "^";
    tan.power()->Accept(*this);
  }
  std::cout << "(";
  tan.expr()->Accept(*this);
  std::cout << ")";
}
