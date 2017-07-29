%skeleton "lalr1.cc"                          /*  -*- C++ -*- */
%require "2.4"
%defines
%define parser_class_name "DerivParser"
%define lr.keep_unreachable_states "true"

%code requires {
  #include <string>
  
  class ParserCtx;//forward declaration
  #define YYMAXDEPTH 2000000
  #define YYINITDEPTH YYMAXDEPTH
}

%parse-param { ParserCtx& driver }
%lex-param   { ParserCtx& driver }
%locations

%debug
%error-verbose

%code requires {
  #include "../parser_objects/common_incl.hpp"
}

%code provides{
  #include "ParserContext.h"
}

//token typename id
%token KW_TERMINATION 0 "End of file"
%token KW_LEXER_ERROR "lexer error"
%token KW_MUL "*"
%token KW_DIV "/"
%token KW_PLUS "+"
%token KW_MINUS "-"
%token KW_POWER "^"
%token KW_COS "cos"
%token KW_SIN "sin"
%token KW_TAN "tan"
%token KW_E_IDENTIFIER "e"
%token KW_PI_IDENTIFIER "Pi"
%token KW_DOT "."
%token KW_LPAREN "("
%token KW_RPAREN ")"
%token KW_NUMBERS "NUMBERS"
%token KW_IDENTIFIER "IDENTIFIER"
%token KW_CONSTANTS_IDENTIFIER "CONSTANT_IDENTIFIER"
%token KW_DOUBLE_LITERAL "DOUBLE_LITERAL"

//lowest
%left KW_PLUS KW_MINUS
%right KW_SIN KW_COS KW_TAN
%left KW_MUL KW_DIV
%left KW_POWER
//%right opt_sign
//highest

%union {
  std::wstring  *str;
  expr_node  *expr_ptr;
  constant<unsigned int> *int_cnst;
  constant<double>  *double_cnst;
//  TRIG_FUNC;
/*
  std::wstring str;
  std::shared_ptr<expr_node> expr_ptr;
#if 0
  std::shared_ptr<bin_op_node> bin_op;
  std::shared_ptr<unary_op> un_op;
  std::shared_ptr<identifier> ident;
#endif
  std::shared_ptr<constant<unsigned int>> int_cnst;
  std::shared_ptr<constant<double>> double_cnst;

#if 0
  std::shared_ptr<constant_variable> cnst_var;
  std::shared_ptr<trigonometric_expr> trig_func;
#endif
*/
}


%type <str> KW_IDENTIFIER KW_CONSTANTS_IDENTIFIER KW_NUMBERS KW_DOUBLE_LITERAL
%type <int_cnst> unsigned_integer
%type <double_cnst> double_value
//%type <TRIG_FUNC> trig_function
%type <expr_ptr> number
%type <expr_ptr> atomic_expr power_expr unary_expr mul_div_expr add_sub_expr expr math_function

%start math_function

%destructor { } "AST Node destructor"

%%
unsigned_integer:
	KW_NUMBERS
		{ 
		  //$$ = std::make_shared<constant<unsigned int>>($1);
		  $$ = new constant<unsigned int>(std::stoul(*$1));
		  delete $1;
		}
	;
	
double_value:
	KW_DOUBLE_LITERAL
		{ 
		  //$$ = std::make_shared<constant<double>>($1);
		  $$ = new constant<double>(std::stod(*$1));
		  delete $1;
		}
	;
	
number:
	unsigned_integer
		{ $$ = $1; }
	| double_value
		{ 
		  //$$ = std::make_shared<constant<double>>($1);
		  $$ = $1;
		}
	;
	
/* (-x)^2 or (-x^2)^3 */
	
atomic_expr: 
	number
		{ $$ = $1; }
	| KW_IDENTIFIER
		{ 
		  //$$ = std::make_shared<identifier>($1);
		  $$ = new identifier(*$1);
		delete $1;
		}
	| KW_CONSTANTS_IDENTIFIER
		{ 
		  //$$ = std::make_shared<constant_variable>($1);
		  $$ = new constant_variable(*$1);
		delete $1;
		} 
	| KW_LPAREN expr KW_RPAREN
		{ $$ = new bracket_expr(std::shared_ptr<expr_node>($2)); }
	| KW_SIN atomic_expr
		{ 
		  $$ = new trigonometric_expr<TRIG_FUNC::SIN>(std::shared_ptr<expr_node>($2));
		}
	| KW_SIN KW_POWER atomic_expr KW_LPAREN expr KW_RPAREN
		{
		  $$ = new trigonometric_expr<TRIG_FUNC::SIN>(std::shared_ptr<expr_node>($5), std::shared_ptr<expr_node>($3));
		}
	| KW_COS atomic_expr
		{ 
		  $$ = new trigonometric_expr<TRIG_FUNC::COS>(std::shared_ptr<expr_node>($2));
		}
	| KW_COS KW_POWER atomic_expr KW_LPAREN expr KW_RPAREN
		{ 
		  $$ = new trigonometric_expr<TRIG_FUNC::COS>(std::shared_ptr<expr_node>($5), std::shared_ptr<expr_node>($3));
		}

	| KW_TAN atomic_expr
		{ 
		  $$ = new trigonometric_expr<TRIG_FUNC::TAN>(std::shared_ptr<expr_node>($2));
		}
	| KW_TAN KW_POWER atomic_expr KW_LPAREN expr KW_RPAREN
		{ 
		  $$ = new trigonometric_expr<TRIG_FUNC::TAN>(std::shared_ptr<expr_node>($5), std::shared_ptr<expr_node>($3));
		}
	;

/*
trig_function:
	KW_SIN { $$ = TRIG_FUNC::SIN; }
	| KW_COS { $$ = TRIG_FUNC::COS; }
	| KW_TAN { $$ = TRIG_FUNC::TAN; }
	;
*/
	
power_expr:
	atomic_expr
		{ $$ = $1; }
	| power_expr KW_POWER atomic_expr
		{ 
		  //$$ = std::make_shared<bin_op_node>($1, bin_op_node::POWER, $3);
		  $$ = new bin_op_node(std::shared_ptr<expr_node>($1), bin_op_node::POWER, std::shared_ptr<expr_node>($3));
		}
/*
	| KW_SIN expr 
		{ $$ = new trigonometric_expr<TRIG_FUNC::SIN>(std::shared_ptr<expr_node>($2)); }
	| KW_SIN KW_POWER expr expr
		{ $$ = new trigonometric_expr<TRIG_FUNC::SIN>(std::shared_ptr<expr_node>($3), std::shared_ptr<expr_node>($4)); }
	| KW_COS expr
		{ $$ = new trigonometric_expr<TRIG_FUNC::COS>(std::shared_ptr<expr_node>($2)); }
	| KW_COS KW_POWER expr expr
		{ $$ = new trigonometric_expr<TRIG_FUNC::COS>(std::shared_ptr<expr_node>($3), std::shared_ptr<expr_node>($4)); }
	| KW_TAN expr
		{ $$ = new trigonometric_expr<TRIG_FUNC::TAN>(std::shared_ptr<expr_node>($2)); }
	| KW_TAN KW_POWER expr expr
		{ $$ = new trigonometric_expr<TRIG_FUNC::TAN>(std::shared_ptr<expr_node>($3), std::shared_ptr<expr_node>($4)); }
*/
	;
	
unary_expr:
	power_expr
		{ $$ = $1; }
	| KW_PLUS power_expr
		{ $$ = $2; }
	| KW_MINUS power_expr
		{ 
		  //$$ = std::make_shared<unary_op>($2, unary_op::MINUS);
		  $$ = new unary_op(std::shared_ptr<expr_node>($2), unary_op::MINUS);
		}
	;
	
mul_div_expr:
	unary_expr
		{ $$ = $1; }
	| mul_div_expr KW_MUL unary_expr
		{ 
		  //$$ = std::make_shared<bin_op_node>($1, bin_op_node::MUL, $3);
		  $$ = new bin_op_node(std::shared_ptr<expr_node>($1), bin_op_node::MUL, std::shared_ptr<expr_node>($3));
		}
	| mul_div_expr KW_DIV unary_expr
		{ 
		  //$$ = std::make_shared<bin_op_node>($1, bin_op_node::DIV, $3);
		  $$ = new bin_op_node(std::shared_ptr<expr_node>($1), bin_op_node::DIV, std::shared_ptr<expr_node>($3));
		} 
	;
	
add_sub_expr:
	mul_div_expr
		{ $$ = $1; }
	| add_sub_expr KW_PLUS mul_div_expr
		{ 
		  //$$ = std::make_shared<bin_op_node>($1, bin_op_node::PLUS, $3);
		  $$ = new bin_op_node(std::shared_ptr<expr_node>($1), bin_op_node::PLUS, std::shared_ptr<expr_node>($3));
		}
	| add_sub_expr KW_MINUS mul_div_expr
		{ 
		  //$$ = std::make_shared<bin_op_node>($1, bin_op_node::MINUS, $3);
		  $$ = new bin_op_node(std::shared_ptr<expr_node>($1), bin_op_node::MINUS, std::shared_ptr<expr_node>($3));
		}
	;
	
expr:
	add_sub_expr
		{ $$ = $1; }
	;
	
math_function:
		expr { driver.setAST(std::shared_ptr<expr_node>($1)); }
	;	
%%

void yy::DerivParser::error(const yy::DerivParser::location_type  &loc, const std::string  &m) 
{
  driver.error(loc, m);
}


