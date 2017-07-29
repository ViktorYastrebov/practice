#pragma once

#include <list>

#include "Expression.h"

class ObjectList : public Expression {
public:
  ObjectList();
  virtual ~ObjectList();

  LOKI_DEFINE_VISITABLE();

  const std::list<Expression  *>  &getList() const;
  std::list<Expression  *>  &getList();

private:
  std::list<Expression  *> m_list;
};