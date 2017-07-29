#pragma once

#include "expr_node.h"
#include <string>

class identifier : public expr_node {
public:
  identifier();
  identifier(const std::wstring  &ident);

  virtual ~identifier();
  virtual AST_NODE_TYPE id() const;

  std::wstring  &get();
  const std::wstring  &get() const;

  std::string toString() const;

  LOKI_DEFINE_VISITABLE();
private:
  std::wstring ident_;
};