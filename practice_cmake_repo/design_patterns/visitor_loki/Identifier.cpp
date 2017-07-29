#include "Identifier.h"

Identifier::Identifier()
  :Expression()
{}

Identifier::Identifier(const std::string  &ident)
  : Expression(), m_ident(ident)
{}

const std::string  &Identifier::getIdentifier() const {
  return m_ident;
}

std::string  &Identifier::getIdentifier() {
  return m_ident;
}