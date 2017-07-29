#include "ObjectList.h"

ObjectList::ObjectList()
  :Expression()
{}

ObjectList::~ObjectList()
{
  for (Expression *i : m_list) {
    delete i;
  }
}

const std::list<Expression  *>  &ObjectList::getList() const {
  return m_list;
}

std::list<Expression  *>  &ObjectList::getList() {
  return m_list;
}