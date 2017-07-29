#pragma once


#include <loki/Visitor.h>

class Expression : public Loki::BaseVisitable<>
{
public:
  virtual ~Expression()
  {}
  LOKI_DEFINE_VISITABLE();
};