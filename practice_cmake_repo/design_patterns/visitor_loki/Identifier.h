#pragma once

#include <string>

#include "Expression.h"

class Identifier : public Expression {
public:
  Identifier();
  Identifier(const std::string  &ident);

  LOKI_DEFINE_VISITABLE();

  const std::string  &getIdentifier() const;
  std::string  &getIdentifier();

private:
  std::string m_ident;
};