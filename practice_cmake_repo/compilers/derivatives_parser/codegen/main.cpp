#include <iostream>

#include "../parser/DerivParser.hpp"
#include "../parser/ParserContext.h"
#include "../parser_objects/derivative_builder.hpp"

#include "codegen.hpp"

auto main() -> int {
  std::string path("d:\\sdks\\vc14\\llvm-3.9\\debug\\bin64\\deriv_codegen.bc");
  
  std::wstring  str_test(L"(x + 1)^2");
  ParserCtx parseCtx;
  int res = parseCtx.parse_string(str_test);

  derivative_builder<state::general> builder(parseCtx.getAST());
  try {
    builder.process();
  } catch (const std::exception  &ex) {
    std::cout << "error occurs :" << ex.what() << std::endl;
  }
  
  CodeGen codeGen(builder.get_deriv_expr());
  codeGen.BuildByteCode();
  //llvm - dis.exe - o = deriv_codegen.dis deriv_codegen.bc
  codeGen.dumpModule(path);

  return 0;
}
