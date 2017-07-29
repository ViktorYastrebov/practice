#pragma once

#define YY_DECL yy::DerivParser::token_type yylex(yy::DerivParser::semantic_type* yylval,               \
              yy::DerivParser::location_type* yylloc,ParserCtx& driver)
        YY_DECL;

#include "../lexer/DerivLexer"
#include <fstream>
#include <sstream>
#include <memory>
#include "../parser_objects/base_ast_node.h"

class ParserCtx {
public:
  ParserCtx();
  virtual ~ParserCtx();

  int parse_string(const std::wstring& f);
  int parser_stream(const std::string  &file);

  bool trace_scanning;
  bool trace_parsing;

  // Error handling.
  void error(const yy::DerivParser::location_type& l, const std::string& m);
  void error(const std::string& m);
  quex::DerivLexer  &lexer();
  std::shared_ptr<base_ast_node>  getAST();
  void setAST(const std::shared_ptr<base_ast_node>  &base_node);

private:
  std::shared_ptr<base_ast_node> base_;
  std::unique_ptr<quex::DerivLexer> lexer_;
};