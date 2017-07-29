#pragma once

#include "Identifier.h"
#include "BinaryOperator.h"
#include "ObjectList.h"

class IdentifierGetter :
  public Loki::BaseVisitor,
  public Loki::Visitor< Expression >,
  public Loki::Visitor< Identifier >,
  public Loki::Visitor< BinaryOperator >,
  public Loki::Visitor< ObjectList >
{
public:

  IdentifierGetter();
  ~IdentifierGetter();

  void Visit(Identifier  &ident);
  void Visit(BinaryOperator  &binOp);
  void Visit(ObjectList  &objList);
  void Visit(Expression  &expr);

  Identifier  *getIdentifier() const;
private:
  Identifier  *m_ident;
};
