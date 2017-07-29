#include <iostream>

#include "DerivParser.hpp"
#include "ParserContext.h"


ParserCtx::ParserCtx()
  :trace_scanning(false), trace_parsing(false)
{}

ParserCtx::~ParserCtx()
{}

int ParserCtx::parse_string(const std::wstring& str) {
  std::wstringstream wstream;
  wstream << str;

  lexer_.reset(new quex::DerivLexer(&wstream));
  yy::DerivParser parser(*this);
  parser.set_debug_level(trace_parsing);
  int result = parser.parse();
  lexer_.reset();
  return result;
}

int ParserCtx::parser_stream(const std::string  &file) {
  
  std::ifstream ifs(file);
  if (!ifs) {
    throw std::runtime_error("can't open file");
  }
  lexer_.reset(new quex::DerivLexer(&ifs, "UTF_16"));
  yy::DerivParser parser(*this);
  parser.set_debug_level(trace_parsing);
  int result = parser.parse();
  lexer_.reset();
  return result;
}

// Error handling.
void ParserCtx::error(const yy::DerivParser::location_type& l, const std::string& m) {
  std::cerr << "location: " << l << " source: " << m << std::endl;
}

void ParserCtx::error(const std::string& m) {
  std::cerr << "source :" << m << std::endl;
}

quex::DerivLexer  &ParserCtx::lexer() {
  return *lexer_;
}

/*
base_ast_node  *ParserCtx::getAST() {
  return result_;
}
const base_ast_node  *ParserCtx::getAST() const {
  return result_;
}*/

std::shared_ptr<base_ast_node>  ParserCtx::getAST() {
  return base_;
}

void ParserCtx::setAST(const std::shared_ptr<base_ast_node>  &base_node) {
  base_ = base_node;
}

yy::DerivParser::token_type yylex(yy::DerivParser::semantic_type* yylval, yy::DerivParser::location_type* yylloc, ParserCtx& driver)
{
  quex::Token* token_p = 0;
  driver.lexer().receive(&token_p);

  if (!token_p || token_p->type_id() == yy::DerivParser::token::KW_TERMINATION)
  {
    return yy::DerivParser::token::KW_TERMINATION;
  }

  yylloc->begin.line = token_p->line_number();
  yylloc->end.line = token_p->line_number();
  yylloc->begin.column = token_p->column_number();
  yylloc->end.column = token_p->column_number() + token_p->pretty_wchar_text().length();

  if (!token_p->get_text().empty()) {
    yylval->str = new std::wstring(token_p->pretty_wchar_text());
  }

  /*
   this can manipulate the runtime partial derivitives option. The complexety is O(logn)
   std::set<std::string>::const_iterator it = variables.find(yylval->str);
   if(it != variables.end()) {
     return yy::DerivParser::token_type(KW_CONSTANTS_IDENTIFIER);
   }
  */
  
  if (driver.trace_scanning)
  {
    std::cout << "token :" << token_p->type_id() << " val:" << token_p->get_string() << std::endl;
  }
  return yy::DerivParser::token_type(token_p->type_id());
}
