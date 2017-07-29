
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* First part of user declarations.  */


/* Line 311 of lalr1.cc  */
#line 41 "DerivParser.cpp"


#include "DerivParser.hpp"

/* User implementation prologue.  */


/* Line 317 of lalr1.cc  */
#line 50 "DerivParser.cpp"

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


/* Line 380 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

namespace yy {

/* Line 380 of lalr1.cc  */
#line 119 "DerivParser.cpp"
#if YYERROR_VERBOSE

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  DerivParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              /* Fall through.  */
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

#endif

  /// Build a parser object.
  DerivParser::DerivParser (ParserCtx& driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg)
  {
  }

  DerivParser::~DerivParser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  DerivParser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  DerivParser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  DerivParser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
        case 21: /* "\"AST Node destructor\"" */

/* Line 480 of lalr1.cc  */
#line 93 "original_gramma.yy"
	{ };

/* Line 480 of lalr1.cc  */
#line 226 "DerivParser.cpp"
	break;

	default:
	  break;
      }
  }

  void
  DerivParser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  DerivParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  DerivParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  DerivParser::debug_level_type
  DerivParser::debug_level () const
  {
    return yydebug_;
  }

  void
  DerivParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  int
  DerivParser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    semantic_type yylval;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[2];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yyn == yypact_ninf_)
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, &yylloc, driver);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yyn == 0 || yyn == yytable_ninf_)
	goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 2:

/* Line 678 of lalr1.cc  */
#line 98 "original_gramma.yy"
    { 
		  //$$ = std::make_shared<constant<unsigned int>>($1);
		  (yyval.int_cnst) = new constant<unsigned int>(std::stoul(*(yysemantic_stack_[(1) - (1)].str)));
		  delete (yysemantic_stack_[(1) - (1)].str);
		}
    break;

  case 3:

/* Line 678 of lalr1.cc  */
#line 107 "original_gramma.yy"
    { 
		  //$$ = std::make_shared<constant<double>>($1);
		  (yyval.double_cnst) = new constant<double>(std::stod(*(yysemantic_stack_[(1) - (1)].str)));
		  delete (yysemantic_stack_[(1) - (1)].str);
		}
    break;

  case 4:

/* Line 678 of lalr1.cc  */
#line 116 "original_gramma.yy"
    { (yyval.expr_ptr) = (yysemantic_stack_[(1) - (1)].int_cnst); }
    break;

  case 5:

/* Line 678 of lalr1.cc  */
#line 118 "original_gramma.yy"
    { 
		  //$$ = std::make_shared<constant<double>>($1);
		  (yyval.expr_ptr) = (yysemantic_stack_[(1) - (1)].double_cnst);
		}
    break;

  case 6:

/* Line 678 of lalr1.cc  */
#line 128 "original_gramma.yy"
    { (yyval.expr_ptr) = (yysemantic_stack_[(1) - (1)].expr_ptr); }
    break;

  case 7:

/* Line 678 of lalr1.cc  */
#line 130 "original_gramma.yy"
    { 
		  //$$ = std::make_shared<identifier>($1);
		  (yyval.expr_ptr) = new identifier(*(yysemantic_stack_[(1) - (1)].str));
		delete (yysemantic_stack_[(1) - (1)].str);
		}
    break;

  case 8:

/* Line 678 of lalr1.cc  */
#line 136 "original_gramma.yy"
    { 
		  //$$ = std::make_shared<constant_variable>($1);
		  (yyval.expr_ptr) = new constant_variable(*(yysemantic_stack_[(1) - (1)].str));
		delete (yysemantic_stack_[(1) - (1)].str);
		}
    break;

  case 9:

/* Line 678 of lalr1.cc  */
#line 142 "original_gramma.yy"
    { (yyval.expr_ptr) = new bracket_expr(std::shared_ptr<expr_node>((yysemantic_stack_[(3) - (2)].expr_ptr))); }
    break;

  case 10:

/* Line 678 of lalr1.cc  */
#line 144 "original_gramma.yy"
    { 
		  (yyval.expr_ptr) = new trigonometric_expr<TRIG_FUNC::SIN>(std::shared_ptr<expr_node>((yysemantic_stack_[(2) - (2)].expr_ptr)));
		}
    break;

  case 11:

/* Line 678 of lalr1.cc  */
#line 148 "original_gramma.yy"
    {
		  (yyval.expr_ptr) = new trigonometric_expr<TRIG_FUNC::SIN>(std::shared_ptr<expr_node>((yysemantic_stack_[(6) - (5)].expr_ptr)), std::shared_ptr<expr_node>((yysemantic_stack_[(6) - (3)].expr_ptr)));
		}
    break;

  case 12:

/* Line 678 of lalr1.cc  */
#line 152 "original_gramma.yy"
    { 
		  (yyval.expr_ptr) = new trigonometric_expr<TRIG_FUNC::COS>(std::shared_ptr<expr_node>((yysemantic_stack_[(2) - (2)].expr_ptr)));
		}
    break;

  case 13:

/* Line 678 of lalr1.cc  */
#line 156 "original_gramma.yy"
    { 
		  (yyval.expr_ptr) = new trigonometric_expr<TRIG_FUNC::COS>(std::shared_ptr<expr_node>((yysemantic_stack_[(6) - (5)].expr_ptr)), std::shared_ptr<expr_node>((yysemantic_stack_[(6) - (3)].expr_ptr)));
		}
    break;

  case 14:

/* Line 678 of lalr1.cc  */
#line 161 "original_gramma.yy"
    { 
		  (yyval.expr_ptr) = new trigonometric_expr<TRIG_FUNC::TAN>(std::shared_ptr<expr_node>((yysemantic_stack_[(2) - (2)].expr_ptr)));
		}
    break;

  case 15:

/* Line 678 of lalr1.cc  */
#line 165 "original_gramma.yy"
    { 
		  (yyval.expr_ptr) = new trigonometric_expr<TRIG_FUNC::TAN>(std::shared_ptr<expr_node>((yysemantic_stack_[(6) - (5)].expr_ptr)), std::shared_ptr<expr_node>((yysemantic_stack_[(6) - (3)].expr_ptr)));
		}
    break;

  case 16:

/* Line 678 of lalr1.cc  */
#line 180 "original_gramma.yy"
    { (yyval.expr_ptr) = (yysemantic_stack_[(1) - (1)].expr_ptr); }
    break;

  case 17:

/* Line 678 of lalr1.cc  */
#line 182 "original_gramma.yy"
    { 
		  //$$ = std::make_shared<bin_op_node>($1, bin_op_node::POWER, $3);
		  (yyval.expr_ptr) = new bin_op_node(std::shared_ptr<expr_node>((yysemantic_stack_[(3) - (1)].expr_ptr)), bin_op_node::POWER, std::shared_ptr<expr_node>((yysemantic_stack_[(3) - (3)].expr_ptr)));
		}
    break;

  case 18:

/* Line 678 of lalr1.cc  */
#line 204 "original_gramma.yy"
    { (yyval.expr_ptr) = (yysemantic_stack_[(1) - (1)].expr_ptr); }
    break;

  case 19:

/* Line 678 of lalr1.cc  */
#line 206 "original_gramma.yy"
    { (yyval.expr_ptr) = (yysemantic_stack_[(2) - (2)].expr_ptr); }
    break;

  case 20:

/* Line 678 of lalr1.cc  */
#line 208 "original_gramma.yy"
    { 
		  //$$ = std::make_shared<unary_op>($2, unary_op::MINUS);
		  (yyval.expr_ptr) = new unary_op(std::shared_ptr<expr_node>((yysemantic_stack_[(2) - (2)].expr_ptr)), unary_op::MINUS);
		}
    break;

  case 21:

/* Line 678 of lalr1.cc  */
#line 216 "original_gramma.yy"
    { (yyval.expr_ptr) = (yysemantic_stack_[(1) - (1)].expr_ptr); }
    break;

  case 22:

/* Line 678 of lalr1.cc  */
#line 218 "original_gramma.yy"
    { 
		  //$$ = std::make_shared<bin_op_node>($1, bin_op_node::MUL, $3);
		  (yyval.expr_ptr) = new bin_op_node(std::shared_ptr<expr_node>((yysemantic_stack_[(3) - (1)].expr_ptr)), bin_op_node::MUL, std::shared_ptr<expr_node>((yysemantic_stack_[(3) - (3)].expr_ptr)));
		}
    break;

  case 23:

/* Line 678 of lalr1.cc  */
#line 223 "original_gramma.yy"
    { 
		  //$$ = std::make_shared<bin_op_node>($1, bin_op_node::DIV, $3);
		  (yyval.expr_ptr) = new bin_op_node(std::shared_ptr<expr_node>((yysemantic_stack_[(3) - (1)].expr_ptr)), bin_op_node::DIV, std::shared_ptr<expr_node>((yysemantic_stack_[(3) - (3)].expr_ptr)));
		}
    break;

  case 24:

/* Line 678 of lalr1.cc  */
#line 231 "original_gramma.yy"
    { (yyval.expr_ptr) = (yysemantic_stack_[(1) - (1)].expr_ptr); }
    break;

  case 25:

/* Line 678 of lalr1.cc  */
#line 233 "original_gramma.yy"
    { 
		  //$$ = std::make_shared<bin_op_node>($1, bin_op_node::PLUS, $3);
		  (yyval.expr_ptr) = new bin_op_node(std::shared_ptr<expr_node>((yysemantic_stack_[(3) - (1)].expr_ptr)), bin_op_node::PLUS, std::shared_ptr<expr_node>((yysemantic_stack_[(3) - (3)].expr_ptr)));
		}
    break;

  case 26:

/* Line 678 of lalr1.cc  */
#line 238 "original_gramma.yy"
    { 
		  //$$ = std::make_shared<bin_op_node>($1, bin_op_node::MINUS, $3);
		  (yyval.expr_ptr) = new bin_op_node(std::shared_ptr<expr_node>((yysemantic_stack_[(3) - (1)].expr_ptr)), bin_op_node::MINUS, std::shared_ptr<expr_node>((yysemantic_stack_[(3) - (3)].expr_ptr)));
		}
    break;

  case 27:

/* Line 678 of lalr1.cc  */
#line 246 "original_gramma.yy"
    { (yyval.expr_ptr) = (yysemantic_stack_[(1) - (1)].expr_ptr); }
    break;

  case 28:

/* Line 678 of lalr1.cc  */
#line 250 "original_gramma.yy"
    { driver.setAST(std::shared_ptr<expr_node>((yysemantic_stack_[(1) - (1)].expr_ptr))); }
    break;



/* Line 678 of lalr1.cc  */
#line 657 "DerivParser.cpp"
	default:
          break;
      }
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[0] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[0] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (yyn != yypact_ninf_)
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[0] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[1] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc);

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  DerivParser::yysyntax_error_ (int yystate, int tok)
  {
    std::string res;
    YYUSE (yystate);
#if YYERROR_VERBOSE
    int yyn = yypact_[yystate];
    if (yypact_ninf_ < yyn && yyn <= yylast_)
      {
	/* Start YYX at -YYN if negative to avoid negative indexes in
	   YYCHECK.  */
	int yyxbegin = yyn < 0 ? -yyn : 0;

	/* Stay within bounds of both yycheck and yytname.  */
	int yychecklim = yylast_ - yyn + 1;
	int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
	int count = 0;
	for (int x = yyxbegin; x < yyxend; ++x)
	  if (yycheck_[x + yyn] == x && x != yyterror_)
	    ++count;

	// FIXME: This method of building the message is not compatible
	// with internationalization.  It should work like yacc.c does it.
	// That is, first build a string that looks like this:
	// "syntax error, unexpected %s or %s or %s"
	// Then, invoke YY_ on this string.
	// Finally, use the string as a format to output
	// yytname_[tok], etc.
	// Until this gets fixed, this message appears in English only.
	res = "syntax error, unexpected ";
	res += yytnamerr_ (yytname_[tok]);
	if (count < 5)
	  {
	    count = 0;
	    for (int x = yyxbegin; x < yyxend; ++x)
	      if (yycheck_[x + yyn] == x && x != yyterror_)
		{
		  res += (!count++) ? ", expecting " : " or ";
		  res += yytnamerr_ (yytname_[x]);
		}
	  }
      }
    else
#endif
      res = YY_("syntax error");
    return res;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char DerivParser::yypact_ninf_ = -10;
  const signed char
  DerivParser::yypact_[] =
  {
        -1,    65,    65,    27,    40,    53,    -1,   -10,   -10,   -10,
     -10,   -10,   -10,   -10,   -10,    -4,   -10,     7,    20,   -10,
      13,    -4,    -4,    65,   -10,    65,   -10,    65,   -10,    -9,
      65,    -1,    -1,    -1,    -1,   -10,     5,     9,    18,   -10,
     -10,   -10,   -10,     7,     7,    -1,    -1,    -1,     6,    38,
      49,   -10,   -10,   -10
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  DerivParser::yydefact_[] =
  {
         0,     0,     0,     0,     0,     0,     0,     2,     7,     8,
       3,     4,     5,     6,    16,    18,    21,    24,    27,    28,
       0,    19,    20,     0,    12,     0,    10,     0,    14,     0,
       0,     0,     0,     0,     0,     1,     0,     0,     0,     9,
      17,    22,    23,    25,    26,     0,     0,     0,     0,     0,
       0,    13,    11,    15
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  DerivParser::yypgoto_[] =
  {
       -10,   -10,   -10,   -10,    -2,    28,     0,    19,   -10,    -6,
     -10
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  DerivParser::yydefgoto_[] =
  {
        -1,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char DerivParser::yytable_ninf_ = -1;
  const unsigned char
  DerivParser::yytable_[] =
  {
        29,    24,    26,    28,    30,     1,     2,    39,     3,     4,
       5,    31,    32,    35,     6,     0,     7,     8,     9,    10,
      45,    36,    51,    37,    46,    38,    33,    34,    40,    21,
      22,    41,    42,    47,     0,    23,     3,     4,     5,    48,
      49,    50,     6,     0,     7,     8,     9,    10,    25,     3,
       4,     5,    43,    44,    52,     6,     0,     7,     8,     9,
      10,    27,     3,     4,     5,    53,     0,     0,     6,     0,
       7,     8,     9,    10,     3,     4,     5,     0,     0,     0,
       6,     0,     7,     8,     9,    10
  };

  /* YYCHECK.  */
  const signed char
  DerivParser::yycheck_[] =
  {
         6,     3,     4,     5,     8,     6,     7,    16,     9,    10,
      11,     4,     5,     0,    15,    -1,    17,    18,    19,    20,
      15,    23,    16,    25,    15,    27,     6,     7,    30,     1,
       2,    31,    32,    15,    -1,     8,     9,    10,    11,    45,
      46,    47,    15,    -1,    17,    18,    19,    20,     8,     9,
      10,    11,    33,    34,    16,    15,    -1,    17,    18,    19,
      20,     8,     9,    10,    11,    16,    -1,    -1,    15,    -1,
      17,    18,    19,    20,     9,    10,    11,    -1,    -1,    -1,
      15,    -1,    17,    18,    19,    20
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  DerivParser::yystos_[] =
  {
         0,     6,     7,     9,    10,    11,    15,    17,    18,    19,
      20,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    27,    27,     8,    26,     8,    26,     8,    26,    31,
       8,     4,     5,     6,     7,     0,    26,    26,    26,    16,
      26,    28,    28,    29,    29,    15,    15,    15,    31,    31,
      31,    16,    16,    16
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  DerivParser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  DerivParser::yyr1_[] =
  {
         0,    22,    23,    24,    25,    25,    26,    26,    26,    26,
      26,    26,    26,    26,    26,    26,    27,    27,    28,    28,
      28,    29,    29,    29,    30,    30,    30,    31,    32
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  DerivParser::yyr2_[] =
  {
         0,     2,     1,     1,     1,     1,     1,     1,     1,     3,
       2,     6,     2,     6,     2,     6,     1,     3,     1,     2,
       2,     1,     3,     3,     1,     3,     3,     1,     1
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const DerivParser::yytname_[] =
  {
    "\"End of file\"", "error", "$undefined", "\"lexer error\"", "\"*\"",
  "\"/\"", "\"+\"", "\"-\"", "\"^\"", "\"cos\"", "\"sin\"", "\"tan\"",
  "\"e\"", "\"Pi\"", "\".\"", "\"(\"", "\")\"", "\"NUMBERS\"",
  "\"IDENTIFIER\"", "\"CONSTANT_IDENTIFIER\"", "\"DOUBLE_LITERAL\"",
  "\"AST Node destructor\"", "$accept", "unsigned_integer", "double_value",
  "number", "atomic_expr", "power_expr", "unary_expr", "mul_div_expr",
  "add_sub_expr", "expr", "math_function", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const DerivParser::rhs_number_type
  DerivParser::yyrhs_[] =
  {
        32,     0,    -1,    17,    -1,    20,    -1,    23,    -1,    24,
      -1,    25,    -1,    18,    -1,    19,    -1,    15,    31,    16,
      -1,    10,    26,    -1,    10,     8,    26,    15,    31,    16,
      -1,     9,    26,    -1,     9,     8,    26,    15,    31,    16,
      -1,    11,    26,    -1,    11,     8,    26,    15,    31,    16,
      -1,    26,    -1,    27,     8,    26,    -1,    27,    -1,     6,
      27,    -1,     7,    27,    -1,    28,    -1,    29,     4,    28,
      -1,    29,     5,    28,    -1,    29,    -1,    30,     6,    29,
      -1,    30,     7,    29,    -1,    30,    -1,    31,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  DerivParser::yyprhs_[] =
  {
         0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      21,    24,    31,    34,    41,    44,    51,    53,    57,    59,
      62,    65,    67,    71,    75,    77,    81,    85,    87
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned char
  DerivParser::yyrline_[] =
  {
         0,    97,    97,   106,   115,   117,   127,   129,   135,   141,
     143,   147,   151,   155,   160,   164,   179,   181,   203,   205,
     207,   215,   217,   222,   230,   232,   237,   245,   250
  };

  // Print the state stack on the debug stream.
  void
  DerivParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  DerivParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  DerivParser::token_number_type
  DerivParser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int DerivParser::yyeof_ = 0;
  const int DerivParser::yylast_ = 85;
  const int DerivParser::yynnts_ = 11;
  const int DerivParser::yyempty_ = -2;
  const int DerivParser::yyfinal_ = 35;
  const int DerivParser::yyterror_ = 1;
  const int DerivParser::yyerrcode_ = 256;
  const int DerivParser::yyntokens_ = 22;

  const unsigned int DerivParser::yyuser_token_number_max_ = 276;
  const DerivParser::token_number_type DerivParser::yyundef_token_ = 2;


/* Line 1054 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

} // yy

/* Line 1054 of lalr1.cc  */
#line 1122 "DerivParser.cpp"


/* Line 1056 of lalr1.cc  */
#line 252 "original_gramma.yy"


void yy::DerivParser::error(const yy::DerivParser::location_type  &loc, const std::string  &m) 
{
  driver.error(loc, m);
}



