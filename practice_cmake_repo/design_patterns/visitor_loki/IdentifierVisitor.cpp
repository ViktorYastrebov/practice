#include "identifierVisitor.h"


IdentifierGetter::IdentifierGetter()
  :m_ident(0)
{}

IdentifierGetter::~IdentifierGetter()
{}

void IdentifierGetter::Visit(Identifier  &ident) {
  m_ident = &ident;
}

void IdentifierGetter::Visit(BinaryOperator  &binOp) {
  Expression  *left = binOp.getLeft();
  left->Accept(*this);
  if (m_ident) {
    return;
  }
  Expression  *right = binOp.getRight();
  right->Accept(*this);
}

void IdentifierGetter::Visit(ObjectList  &objList) {
  std::list<Expression  *>  &list = objList.getList();
  std::list<Expression  *>::iterator i = list.begin();
  std::list<Expression  *>::iterator e = list.end();

  for (i; i != e && !m_ident; ++i) {
    (*i)->Accept(*this);
  }
}

void IdentifierGetter::Visit(Expression  &expr) {
  
}

Identifier  *IdentifierGetter::getIdentifier() const {
  return m_ident;
}