#include <iostream>

#include "BinaryOperator.h"
#include "Identifier.h"
#include "ObjectList.h"

#include "identifierVisitor.h"



int main() {
  
  ObjectList  *obList = new ObjectList();
  Identifier  *id1 = new Identifier(std::string("left"));
  Identifier  *id2 = new Identifier(std::string("right"));
  BinaryOperator  *binOp = new BinaryOperator(id1, id2, BinaryOperator::OP_EQL);
  obList->getList().push_back(binOp);

  IdentifierGetter identGetter;
  identGetter.Visit(*obList);

  Identifier  *firstIdent = identGetter.getIdentifier();

  std::cout << firstIdent->getIdentifier() << std::endl;

  delete obList;
  return 0;
}