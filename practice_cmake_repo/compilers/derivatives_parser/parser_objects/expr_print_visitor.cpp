#include "expr_print_visitor.h"

expr_printer::expr_printer()
{}

expr_printer::~expr_printer()
{}


void expr_printer::Visit(base_ast_node  &base) {
  base.Accept(*this);
}

void expr_printer::Visit(expr_node  &expr) {
  expr.Accept(*this);
}

void expr_printer::Visit(bracket_expr &br_expr) {
  br_expr.expr()->Accept(*this);
  std::wstring  &top = results_.top();
  top = std::wstring(L"(") + top + std::wstring(L")");
}

void expr_printer::Visit(constant<unsigned int>  &cnst) {
  results_.push(std::to_wstring(cnst.value()));
}

void expr_printer::Visit(constant<double>  &cnst) {
  results_.push(std::to_wstring(cnst.value()));
}

void expr_printer::Visit(identifier  &ident) {
  results_.push(ident.get());
}

void expr_printer::Visit(constant_variable  &var) {
  results_.push(var.get());
}

void expr_printer::Visit(unary_op  &un) {
  un.expr()->Accept(*this);
}

void expr_printer::Visit(bin_op_node  &bin) {
  bin.get_left()->Accept(*this);
  std::wstring lres = results_.top();
  results_.pop();
  bin.get_right()->Accept(*this);
  std::wstring rres = results_.top();
  results_.pop();
  std::wstring result = lres + bin.wstr_op() + rres;
  results_.push(result);
}

void expr_printer::Visit(trigonometric_expr<TRIG_FUNC::COS>  &cos) {

}

void expr_printer::Visit(trigonometric_expr<TRIG_FUNC::SIN>  &sin) {

}

void expr_printer::Visit(trigonometric_expr<TRIG_FUNC::TAN>  &tan) {

}

std::wstring  expr_printer::get_result() const {
  if (results_.empty() || results_.size() > 1) {
    throw std::logic_error("something went wrong, stack should contain single elem");
  }
  return results_.top();
}