#include <iostream>
#include "auxiliary.h"

void classicFactory() {
  std::shared_ptr<GuiFactory> AbsFactory = std::make_shared<WinGuiFactory>();
  std::cout << AbsFactory->CreateButton()->GetName() << std::endl;
  std::cout << AbsFactory->CreatePicture()->GetPictureName() << std::endl;
}

void mapFactory2() {
  GuiSelector sel;
  std::shared_ptr<GuiFactory> fact = sel.getByName("Mac");
  std::cout << fact->CreateButton()->GetName() << std::endl;
  std::cout << fact->CreatePicture()->GetPictureName() << std::endl;
}

int main()
{
  classicFactory();
  mapFactory2();
  return 0;
}
