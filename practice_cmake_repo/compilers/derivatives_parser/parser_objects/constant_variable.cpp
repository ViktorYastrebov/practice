#include "constant_variable.h"

constant_variable::constant_variable()
  :identifier()
{}

constant_variable::constant_variable(const std::wstring  &name)
  : identifier(name)
{}

constant_variable::AST_NODE_TYPE constant_variable::id() const {
  return CONTANT_VARIABLE;
}

constant_variable::~constant_variable()
{}