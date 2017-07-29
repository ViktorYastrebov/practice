#include "identifier.h"
#include <locale>
#include <codecvt>

identifier::identifier()
  : expr_node()
{}

identifier::identifier(const std::wstring  &ident)
  :ident_(ident)
{}

identifier::~identifier()
{}

identifier::AST_NODE_TYPE identifier::id() const {
  return IDENT;
}

std::wstring  &identifier::get() {
  return ident_;
}

const std::wstring  &identifier::get() const {
  return ident_;
}

std::string identifier::toString() const {
  using convert_typeX = std::codecvt_utf8<wchar_t>;
  std::wstring_convert<convert_typeX, wchar_t> converterX;
  return converterX.to_bytes(ident_);
}