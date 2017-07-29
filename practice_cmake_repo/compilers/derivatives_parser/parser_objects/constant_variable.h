#pragma once
#include "identifier.h"

class constant_variable : public identifier {
public:
  constant_variable();
  constant_variable(const std::wstring  &name);
  virtual AST_NODE_TYPE id() const;
  virtual ~constant_variable();
  LOKI_DEFINE_VISITABLE();
};