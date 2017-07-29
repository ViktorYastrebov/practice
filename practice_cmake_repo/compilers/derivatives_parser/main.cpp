#include <iostream>
#include <sstream>

#include <memory>

#include "../parser/DerivParser.hpp"
#include "parser/ParserContext.h"

#include "../parser/DerivParser.hpp"
//#include "../parser_objects/print_visitor.h"
#include "../parser_objects/derivative_builder.hpp"


std::wstring parser_input(const std::wstring  &wstr) {
  std::wstringstream wss;
  wss << wstr;

  ParserCtx parseCtx;
  int res = parseCtx.parse_string(wss.str());

  derivative_builder<state::general> builder(parseCtx.getAST());
  builder.process();
  expr_printer printer;
  printer.Visit(*builder.get_deriv_expr());
  return printer.get_result();
}


int main(int argc, char * argv[]) {

  std::wstring values[] = {
     L"x",
     L"x + 1",
     L"x^2 + x",
     L"x^2 + x^3 + (x-1)^4",
     L"sin^2(x) + x^2",
     L"cos^3(x+1) - (x+1)^2",
     L"x + (x+1)^3*cos^2(x)"
  };

  std::wcout << parser_input(values[0]) << std::endl;
  std::wcout << parser_input(values[1]) << std::endl;
  std::wcout << parser_input(values[2]) << std::endl;
  std::wcout << parser_input(values[3]) << std::endl;

  //BUG fix for 2*(x+1)^1: must be simplified
  std::wcout << parser_input(L"(x+1)^2") << std::endl;
  return 0;
}
