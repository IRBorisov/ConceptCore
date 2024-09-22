// A Bison parser, made by GNU Bison 3.7.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2020 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

// "%code top" blocks.
#line 24 "RSParserImpl.y"

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 4244 26434 26438 26440 26446 26447 26448 26456 )
  #pragma warning( disable : 26460 26477 26481 26482 26493 26494 26495 26496 )
#endif

#ifdef __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wsign-conversion"
  #pragma clang diagnostic ignored "-Wunused-but-set-variable"
#endif

#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wuseless-cast"
#endif


#line 59 "RSParserImpl.cpp"




#include "RSParserImpl.h"




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 15 "RSParserImpl.y"
namespace ccl { namespace rslang { namespace detail {
#line 140 "RSParserImpl.cpp"

  /// Build a parser object.
  RSParserImpl::RSParserImpl (ParserState* state_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      state (state_yyarg)
  {}

  RSParserImpl::~RSParserImpl ()
  {}

  RSParserImpl::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | symbol kinds.  |
  `---------------*/

  // basic_symbol.
  template <typename Base>
  RSParserImpl::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  RSParserImpl::basic_symbol<Base>::basic_symbol (typename Base::kind_type t)
    : Base (t)
    , value ()
  {}

  template <typename Base>
  RSParserImpl::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (semantic_type) v)
    : Base (t)
    , value (YY_MOVE (v))
  {}

  template <typename Base>
  RSParserImpl::symbol_kind_type
  RSParserImpl::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }

  template <typename Base>
  bool
  RSParserImpl::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  RSParserImpl::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
  }

  // by_kind.
  RSParserImpl::by_kind::by_kind ()
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  RSParserImpl::by_kind::by_kind (by_kind&& that)
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  RSParserImpl::by_kind::by_kind (const by_kind& that)
    : kind_ (that.kind_)
  {}

  RSParserImpl::by_kind::by_kind (token_kind_type t)
    : kind_ (yytranslate_ (t))
  {}

  void
  RSParserImpl::by_kind::clear ()
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  RSParserImpl::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  RSParserImpl::symbol_kind_type
  RSParserImpl::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }

  RSParserImpl::symbol_kind_type
  RSParserImpl::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  // by_state.
  RSParserImpl::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  RSParserImpl::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  RSParserImpl::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  RSParserImpl::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  RSParserImpl::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  RSParserImpl::symbol_kind_type
  RSParserImpl::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  RSParserImpl::stack_symbol_type::stack_symbol_type ()
  {}

  RSParserImpl::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  RSParserImpl::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value))
  {
    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  RSParserImpl::stack_symbol_type&
  RSParserImpl::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    return *this;
  }

  RSParserImpl::stack_symbol_type&
  RSParserImpl::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  RSParserImpl::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YYUSE (yysym.kind ());
  }

#if YYDEBUG
  template <typename Base>
  void
  RSParserImpl::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " (";
        YYUSE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  RSParserImpl::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  RSParserImpl::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  RSParserImpl::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  RSParserImpl::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  RSParserImpl::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  RSParserImpl::debug_level_type
  RSParserImpl::debug_level () const
  {
    return yydebug_;
  }

  void
  RSParserImpl::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  RSParserImpl::state_type
  RSParserImpl::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  RSParserImpl::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  RSParserImpl::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  RSParserImpl::operator() ()
  {
    return parse ();
  }

  int
  RSParserImpl::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value, state));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;


      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 3: // expression: no_declaration
#line 271 "RSParserImpl.y"
                                            { if(!state->FinalizeExpression(yystack_[0].value)) YYABORT; }
#line 591 "RSParserImpl.cpp"
    break;

  case 6: // global_declaration: global_name DEFINE
#line 280 "RSParserImpl.y"
                                            { if(!state->FinalizeCstEmpty(yystack_[1].value, yystack_[0].value)) YYABORT; }
#line 597 "RSParserImpl.cpp"
    break;

  case 7: // global_declaration: global_name DEFINE no_declaration
#line 281 "RSParserImpl.y"
                                            { if(!state->FinalizeCstExpression(yystack_[2].value, yystack_[1].value, yystack_[0].value)) YYABORT; }
#line 603 "RSParserImpl.cpp"
    break;

  case 8: // global_declaration: global_name STRUCT no_declaration
#line 282 "RSParserImpl.y"
                                            { if(!state->FinalizeCstExpression(yystack_[2].value, yystack_[1].value, yystack_[0].value)) YYABORT; }
#line 609 "RSParserImpl.cpp"
    break;

  case 11: // function_definition: LS arguments RS logic_or_setexpr
#line 291 "RSParserImpl.y"
                                            { yylhs.value = FunctionDeclaration(yystack_[3].value, yystack_[2].value, yystack_[0].value); }
#line 615 "RSParserImpl.cpp"
    break;

  case 12: // function_definition: LS error
#line 292 "RSParserImpl.y"
                                            { state->OnError(ParseEID::expectedDeclaration); YYABORT; }
#line 621 "RSParserImpl.cpp"
    break;

  case 13: // arguments: declaration
#line 297 "RSParserImpl.y"
                                            { yylhs.value = AddNode(TokenID::NT_ARGUMENTS, yystack_[0].value); }
#line 627 "RSParserImpl.cpp"
    break;

  case 14: // arguments: arguments COMMA declaration
#line 298 "RSParserImpl.y"
                                            { yylhs.value = Enumeration(TokenID::NT_ARGUMENTS, yystack_[2].value, yystack_[0].value); }
#line 633 "RSParserImpl.cpp"
    break;

  case 15: // arguments: arguments COMMA error
#line 299 "RSParserImpl.y"
                                            { state->OnError(ParseEID::expectedLocal); YYABORT; }
#line 639 "RSParserImpl.cpp"
    break;

  case 16: // declaration: LOCAL IN setexpr
#line 302 "RSParserImpl.y"
                                            { yylhs.value = AddNode(TokenID::NT_ARG_DECL, yystack_[2].value, yystack_[0].value); }
#line 645 "RSParserImpl.cpp"
    break;

  case 17: // declaration: LOCAL error
#line 303 "RSParserImpl.y"
                                            { state->OnError(ParseEID::expectedDeclaration); YYABORT; }
#line 651 "RSParserImpl.cpp"
    break;

  case 19: // variable: tuple
#line 308 "RSParserImpl.y"
                                            { yylhs.value = TupleDeclaration(state, yystack_[0].value); if (!yylhs.value) YYABORT; }
#line 657 "RSParserImpl.cpp"
    break;

  case 21: // variable_pack: variable_pack COMMA variable_pack
#line 312 "RSParserImpl.y"
                                            { yylhs.value = Enumeration(TokenID::NT_ENUM_DECL, yystack_[2].value, yystack_[0].value); }
#line 663 "RSParserImpl.cpp"
    break;

  case 22: // variable_pack: variable_pack COMMA error
#line 313 "RSParserImpl.y"
                                            { state->OnError(ParseEID::expectedLocal); YYABORT; }
#line 669 "RSParserImpl.cpp"
    break;

  case 31: // logic_par: LP logic_binary RPE
#line 332 "RSParserImpl.y"
                                            { yylhs.value = RemoveBrackets(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 675 "RSParserImpl.cpp"
    break;

  case 32: // logic_par: LP logic_predicates RPE
#line 333 "RSParserImpl.y"
                                            { yylhs.value = RemoveBrackets(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 681 "RSParserImpl.cpp"
    break;

  case 33: // logic_predicates: setexpr binary_predicate setexpr
#line 337 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 687 "RSParserImpl.cpp"
    break;

  case 34: // logic_predicates: variable ITERATE setexpr
#line 338 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 693 "RSParserImpl.cpp"
    break;

  case 35: // logic_predicates: variable ASSIGN setexpr
#line 339 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 699 "RSParserImpl.cpp"
    break;

  case 47: // logic_unary: NOT logic_no_binary
#line 356 "RSParserImpl.y"
                                            { yylhs.value = UnaryOperation(yystack_[1].value, yystack_[0].value);}
#line 705 "RSParserImpl.cpp"
    break;

  case 48: // logic_unary: quantifier variable_pack IN setexpr logic_no_binary
#line 357 "RSParserImpl.y"
                                                          { yylhs.value = Quantifier(yystack_[4].value, yystack_[3].value, yystack_[1].value, yystack_[0].value);}
#line 711 "RSParserImpl.cpp"
    break;

  case 49: // logic_unary: quantifier error
#line 358 "RSParserImpl.y"
                                            { state->OnError(ParseEID::invalidQuantifier); YYABORT; }
#line 717 "RSParserImpl.cpp"
    break;

  case 50: // logic_unary: PREDICATE LS setexpr_enum RS
#line 359 "RSParserImpl.y"
                                            { yylhs.value = FunctionCall(yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 723 "RSParserImpl.cpp"
    break;

  case 53: // logic_binary: logic_all EQUIVALENT logic_all
#line 367 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 729 "RSParserImpl.cpp"
    break;

  case 54: // logic_binary: logic_all IMPLICATION logic_all
#line 368 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 735 "RSParserImpl.cpp"
    break;

  case 55: // logic_binary: logic_all OR logic_all
#line 369 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 741 "RSParserImpl.cpp"
    break;

  case 56: // logic_binary: logic_all AND logic_all
#line 370 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 747 "RSParserImpl.cpp"
    break;

  case 61: // setexpr: FUNCTION LS setexpr_enum RS
#line 380 "RSParserImpl.y"
                                            { yylhs.value = FunctionCall(yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 753 "RSParserImpl.cpp"
    break;

  case 62: // setexpr: text_function LP setexpr RPE
#line 381 "RSParserImpl.y"
                                            { yylhs.value = TextOperator(yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 759 "RSParserImpl.cpp"
    break;

  case 69: // setexpr_enum: setexpr
#line 392 "RSParserImpl.y"
                                            { yylhs.value = AddNode(TokenID::INTERRUPT, yystack_[0].value); }
#line 765 "RSParserImpl.cpp"
    break;

  case 71: // setexpr_enum_min2: setexpr_enum COMMA setexpr
#line 396 "RSParserImpl.y"
                                            { yylhs.value = Enumeration(TokenID::INTERRUPT, yystack_[2].value, yystack_[0].value); }
#line 771 "RSParserImpl.cpp"
    break;

  case 81: // setexpr_binary: setexpr PLUS setexpr
#line 418 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 777 "RSParserImpl.cpp"
    break;

  case 82: // setexpr_binary: setexpr MINUS setexpr
#line 419 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 783 "RSParserImpl.cpp"
    break;

  case 83: // setexpr_binary: setexpr MULTIPLY setexpr
#line 420 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 789 "RSParserImpl.cpp"
    break;

  case 84: // setexpr_binary: setexpr UNION setexpr
#line 421 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 795 "RSParserImpl.cpp"
    break;

  case 85: // setexpr_binary: setexpr SET_MINUS setexpr
#line 422 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 801 "RSParserImpl.cpp"
    break;

  case 86: // setexpr_binary: setexpr SYMMINUS setexpr
#line 423 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 807 "RSParserImpl.cpp"
    break;

  case 87: // setexpr_binary: setexpr INTERSECTION setexpr
#line 424 "RSParserImpl.y"
                                            { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 813 "RSParserImpl.cpp"
    break;

  case 88: // setexpr_binary: setexpr DECART setexpr
#line 425 "RSParserImpl.y"
                                            { yylhs.value = Decartian(yystack_[2].value, yystack_[1].value, yystack_[0].value);}
#line 819 "RSParserImpl.cpp"
    break;

  case 89: // setexpr_binary: LP setexpr_binary RPE
#line 426 "RSParserImpl.y"
                                            { yylhs.value = RemoveBrackets(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 825 "RSParserImpl.cpp"
    break;

  case 97: // enumeration: LC setexpr_enum RC
#line 439 "RSParserImpl.y"
                                            { yylhs.value = ReplaceBrackets(TokenID::NT_ENUMERATION, yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 831 "RSParserImpl.cpp"
    break;

  case 98: // tuple: LP setexpr_enum_min2 RPE
#line 442 "RSParserImpl.y"
                                            { yylhs.value = ReplaceBrackets(TokenID::NT_TUPLE, yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 837 "RSParserImpl.cpp"
    break;

  case 99: // boolean: BOOLEAN LP setexpr RPE
#line 445 "RSParserImpl.y"
                                            { yylhs.value = TextOperator(yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 843 "RSParserImpl.cpp"
    break;

  case 100: // boolean: BOOLEAN boolean
#line 446 "RSParserImpl.y"
                                            { yylhs.value = UnaryOperation(yystack_[1].value, yystack_[0].value); }
#line 849 "RSParserImpl.cpp"
    break;

  case 101: // filter_expression: FILTER LS setexpr_enum RS LP setexpr RPE
#line 449 "RSParserImpl.y"
                                               { yylhs.value = FilterCall(yystack_[6].value, yystack_[4].value, yystack_[1].value, yystack_[0].value); }
#line 855 "RSParserImpl.cpp"
    break;

  case 102: // declarative: LC LOCAL IN setexpr BAR logic RCE
#line 453 "RSParserImpl.y"
                                            { yylhs.value = TermDeclaration(yystack_[6].value, yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 861 "RSParserImpl.cpp"
    break;

  case 103: // declarative: DECLARATIVE LC variable IN setexpr BAR logic RCE
#line 454 "RSParserImpl.y"
                                                       { yylhs.value = TermDeclaration(yystack_[7].value, yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 867 "RSParserImpl.cpp"
    break;

  case 104: // recursion: RECURSIVE LC variable ASSIGN setexpr BAR logic BAR setexpr RCE
#line 457 "RSParserImpl.y"
                                                                     { yylhs.value = FullRecursion(yystack_[9].value, yystack_[7].value, yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 873 "RSParserImpl.cpp"
    break;

  case 105: // recursion: RECURSIVE LC variable ASSIGN setexpr BAR setexpr RCE
#line 458 "RSParserImpl.y"
                                                           { yylhs.value = ShortRecursion(yystack_[7].value, yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 879 "RSParserImpl.cpp"
    break;

  case 106: // imperative: IMPERATIVE LC setexpr BAR imp_blocks RCE
#line 461 "RSParserImpl.y"
                                               { yylhs.value = Imperative(yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 885 "RSParserImpl.cpp"
    break;

  case 107: // imp_blocks: logic
#line 464 "RSParserImpl.y"
                                            { yylhs.value = AddNode(TokenID::INTERRUPT, yystack_[0].value); }
#line 891 "RSParserImpl.cpp"
    break;

  case 108: // imp_blocks: imp_blocks SEMICOLON imp_blocks
#line 465 "RSParserImpl.y"
                                            { yylhs.value = Enumeration(TokenID::INTERRUPT, yystack_[2].value, yystack_[0].value); }
#line 897 "RSParserImpl.cpp"
    break;

  case 110: // RPE: error
#line 472 "RSParserImpl.y"
            { state->OnError(ParseEID::missingParenthesis); YYABORT; }
#line 903 "RSParserImpl.cpp"
    break;

  case 112: // RCE: error
#line 476 "RSParserImpl.y"
            { state->OnError(ParseEID::missingCurlyBrace); YYABORT; }
#line 909 "RSParserImpl.cpp"
    break;


#line 913 "RSParserImpl.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (YY_MOVE (msg));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;


      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  RSParserImpl::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  RSParserImpl::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
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
              else
                goto append;

            append:
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

  std::string
  RSParserImpl::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // RSParserImpl::context.
  RSParserImpl::context::context (const RSParserImpl& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  RSParserImpl::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        int yychecklim = yylast_ - yyn + 1;
        int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }



  int
  RSParserImpl::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  RSParserImpl::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char RSParserImpl::yypact_ninf_ = -113;

  const signed char RSParserImpl::yytable_ninf_ = -108;

  const short
  RSParserImpl::yypact_[] =
  {
     281,   -14,  -113,   -17,    11,  -113,  -113,  -113,  -113,  -113,
    -113,   414,   -25,  -113,  -113,    18,  -113,  -113,  -113,  -113,
      -3,     4,    35,   414,   305,    28,    78,  -113,  -113,  -113,
    -113,    73,   110,   118,  -113,  -113,  -113,     8,  -113,   531,
      44,   103,  -113,  -113,  -113,  -113,  -113,   123,  -113,  -113,
    -113,  -113,  -113,   434,   434,  -113,  -113,  -113,  -113,   531,
    -113,   434,  -113,   434,    16,    16,   434,  -113,    24,   151,
     531,    68,     6,   196,   111,  -113,   434,   488,   -27,  -113,
    -113,  -113,    29,   -41,  -113,  -113,   434,   434,   414,   414,
     414,   414,  -113,  -113,   434,  -113,   -19,  -113,   434,   434,
     434,  -113,  -113,  -113,  -113,  -113,  -113,  -113,  -113,  -113,
    -113,  -113,   434,   434,   434,   434,   434,   434,   434,   281,
     281,  -113,    25,    33,   328,    36,   119,   120,   207,  -113,
    -113,  -113,  -113,   434,  -113,  -113,   434,  -113,  -113,   434,
     414,   146,   488,   488,   137,   155,   152,  -113,   434,    19,
     124,   124,   201,  -113,  -113,  -113,  -113,  -113,   488,   328,
    -113,  -113,  -113,  -113,  -113,   130,   434,   434,   414,   488,
     471,   488,  -113,  -113,  -113,   362,  -113,  -113,  -113,   434,
     478,   483,     2,     9,   414,  -113,   328,   414,   414,  -113,
    -113,   414,  -113,    20,  -113,    20,   128,   245,  -113,  -113,
    -113,   434,  -113,    96,  -113
  };

  const signed char
  RSParserImpl::yydefact_[] =
  {
       0,    79,    72,    73,    74,    80,    75,    77,    76,    51,
      52,     0,     0,    66,    67,     0,    68,    63,    64,    65,
       0,     0,     0,     0,     0,     0,     0,     3,     2,     4,
       5,     0,    29,     0,    30,    23,    24,     0,    25,    10,
       0,    78,    57,    58,    59,    60,    90,    91,    92,    93,
      94,    96,    95,     0,     0,    47,    28,    26,    27,     0,
      78,     0,   100,     0,     0,     0,     0,    29,     0,     0,
      69,     0,     0,     0,    79,    74,     0,    69,     0,    70,
      91,    12,     0,     0,    13,     1,     0,     0,     0,     0,
       0,     0,    49,    18,     0,    20,     0,    19,     0,     0,
       0,    43,    44,    45,    46,    42,    41,    36,    37,    38,
      39,    40,     0,     0,     0,     0,     0,     0,     0,     6,
       0,    79,     0,     0,     0,     0,     0,     0,     0,   110,
     109,    32,    31,     0,    98,    89,     0,    97,    17,     0,
       0,     0,    34,    35,    53,    54,    55,    56,     0,     0,
      81,    82,    83,    88,    84,    87,    85,    86,    33,     0,
       7,     8,    61,    50,    99,     0,     0,     0,     0,    71,
       0,    16,    11,    15,    14,     0,    22,    21,    62,     0,
       0,     0,    29,     0,     0,    48,     0,     0,     0,   112,
     111,     0,   106,     0,   101,     0,    29,     0,   108,   102,
     103,     0,   105,     0,   104
  };

  const short
  RSParserImpl::yypgoto_[] =
  {
    -113,  -113,    65,  -113,    47,  -113,  -113,    50,   -23,    45,
       5,    23,   162,    -7,   -10,  -113,    -5,  -113,   172,     0,
    -113,    48,   -21,   200,  -113,  -113,    -6,  -113,  -113,    15,
     193,  -113,  -113,  -113,  -113,    26,   -36,  -112
  };

  const short
  RSParserImpl::yydefgoto_[] =
  {
      -1,    26,    27,    28,    29,    30,    83,    84,    31,    96,
      67,    55,    33,    34,    35,   117,    36,    37,    38,    59,
      40,    71,    79,    60,    42,    43,    44,    45,    46,    80,
      48,    49,    50,    51,    52,   183,   131,   192
  };

  const short
  RSParserImpl::yytable_[] =
  {
      39,    57,    72,  -107,    56,    32,    58,   129,   148,    92,
     189,    93,    12,    68,    95,    47,   140,    73,   141,    93,
     176,   189,    93,    70,    77,   129,    47,    61,   137,    81,
     138,    82,   133,   132,   -18,   -18,   134,   135,    47,    53,
     149,   126,   127,   -29,   -29,   -29,   -29,   -23,   -23,   -23,
     -23,    64,    97,    77,    77,    72,   139,  -107,    65,   130,
      94,   124,  -107,    77,   190,   -70,   128,    54,    94,   191,
      73,    94,    78,    72,    63,   190,    77,   130,    85,    97,
      97,   199,   162,   200,   133,   202,   142,   143,   164,    66,
     163,   204,   133,   165,    77,   133,   118,   189,   150,   151,
     152,   122,   123,    47,    47,    47,    47,    98,    99,   100,
      -9,   125,   153,   154,   155,   156,   157,   158,   159,    39,
      39,    86,    87,   178,    32,    32,    95,   133,   112,   113,
     114,   115,   116,   169,    47,    47,   170,   100,   136,   171,
      39,    88,    89,    90,    91,    32,   166,   173,   175,    82,
     194,   190,   129,   119,   120,    47,   112,   113,   114,   115,
     116,    89,    90,    91,    97,    57,   180,   181,    56,   167,
      58,   -19,   -19,   182,   -25,   -25,   -25,   -25,    91,   186,
      90,    91,   179,    47,   160,   161,   201,   172,   197,   193,
      47,   174,   195,   196,   177,    69,   182,   129,   185,    47,
      41,   203,    47,    47,   130,    62,    47,   -59,   -59,   -59,
     -59,   -59,   -59,   -59,   -59,   -59,     0,   198,    98,    99,
     100,     0,     0,   -59,   -59,   -59,   -59,   -59,   -59,   -59,
     -59,   -59,   -59,   112,   113,   114,   115,   116,     0,   112,
     113,   114,   115,   116,     0,     0,   189,     0,     0,   130,
     144,   145,   146,   147,     0,   -59,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   168,     0,     0,     0,     0,
       0,     0,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,     0,     0,     1,     2,     3,     4,     5,     6,
       7,     8,     0,     0,     0,     0,     0,     0,     0,     0,
     190,     9,    10,    11,     0,     0,     0,     0,    74,     2,
       3,    75,     5,     6,     7,     8,     0,     0,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,   129,
       0,     0,     0,    23,     0,    24,     0,    25,     0,    98,
      99,   100,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,     0,     0,     0,     0,    76,     0,    24,
     112,   113,   114,   115,   116,     1,     2,     3,     4,     5,
       6,     7,     8,    98,    99,   100,     0,     0,     0,     0,
       0,   130,     9,    10,    11,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   112,   113,   114,   115,   116,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
       0,     0,     0,     0,    23,     0,    24,     1,     2,     3,
       4,     5,     6,     7,     8,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     9,    10,    11,   121,     2,     3,
      75,     5,     6,     7,     8,     0,     0,     0,     0,     0,
       0,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,     0,     0,     0,     0,    23,     0,    24,     0,
       0,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    98,    99,   100,     0,    76,     0,    24,    98,
      99,   100,     0,     0,    98,    99,   100,     0,     0,    98,
      99,   100,     0,   112,   113,   114,   115,   116,     0,     0,
     112,   113,   114,   115,   116,   112,   113,   114,   115,   116,
     112,   113,   114,   115,   116,     0,     0,     0,     0,   184,
       0,     0,     0,     0,     0,     0,   187,     0,     0,     0,
       0,   188,    98,    99,   100,   101,   102,   103,   104,   105,
     106,     0,     0,     0,     0,     0,     0,     0,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116
  };

  const short
  RSParserImpl::yycheck_[] =
  {
       0,    11,    23,     1,    11,     0,    11,     1,    27,     1,
       1,     3,    37,    23,    37,     0,    57,    23,    59,     3,
       1,     1,     3,    23,    24,     1,    11,    52,    55,     1,
       1,     3,    59,    69,    48,    49,    72,    73,    23,    56,
      59,    64,    65,    23,    24,    25,    26,    23,    24,    25,
      26,    54,    37,    53,    54,    76,    27,    55,    54,    53,
      52,    61,    60,    63,    55,    59,    66,    56,    52,    60,
      76,    52,    24,    94,    56,    55,    76,    53,     0,    64,
      65,   193,    57,   195,    59,   197,    86,    87,   124,    54,
      57,   203,    59,    57,    94,    59,    52,     1,    98,    99,
     100,    53,    54,    88,    89,    90,    91,    11,    12,    13,
       0,    63,   112,   113,   114,   115,   116,   117,   118,   119,
     120,    48,    49,   159,   119,   120,   149,    59,    32,    33,
      34,    35,    36,   133,   119,   120,   136,    13,    27,   139,
     140,    23,    24,    25,    26,   140,    27,     1,   148,     3,
     186,    55,     1,    50,    51,   140,    32,    33,    34,    35,
      36,    24,    25,    26,   149,   175,   166,   167,   175,    49,
     175,    48,    49,   168,    23,    24,    25,    26,    26,   179,
      25,    26,    52,   168,   119,   120,    58,   140,   188,   184,
     175,   141,   187,   188,   149,    23,   191,     1,   175,   184,
       0,   201,   187,   188,    53,    12,   191,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    -1,   191,    11,    12,
      13,    -1,    -1,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    32,    33,    34,    35,    36,    -1,    32,
      33,    34,    35,    36,    -1,    -1,     1,    -1,    -1,    53,
      88,    89,    90,    91,    -1,    59,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    20,    21,    22,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,     7,     8,     9,    10,    -1,    -1,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,     1,
      -1,    -1,    -1,    52,    -1,    54,    -1,    56,    -1,    11,
      12,    13,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    -1,    -1,    -1,    -1,    52,    -1,    54,
      32,    33,    34,    35,    36,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    -1,    -1,    -1,    -1,
      -1,    53,    20,    21,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      -1,    -1,    -1,    -1,    52,    -1,    54,     3,     4,     5,
       6,     7,     8,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    20,    21,    22,     3,     4,     5,
       6,     7,     8,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    -1,    -1,    -1,    -1,    52,    -1,    54,    -1,
      -1,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    11,    12,    13,    -1,    52,    -1,    54,    11,
      12,    13,    -1,    -1,    11,    12,    13,    -1,    -1,    11,
      12,    13,    -1,    32,    33,    34,    35,    36,    -1,    -1,
      32,    33,    34,    35,    36,    32,    33,    34,    35,    36,
      32,    33,    34,    35,    36,    -1,    -1,    -1,    -1,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    58,    -1,    -1,    -1,
      -1,    58,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36
  };

  const signed char
  RSParserImpl::yystos_[] =
  {
       0,     3,     4,     5,     6,     7,     8,     9,    10,    20,
      21,    22,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    52,    54,    56,    62,    63,    64,    65,
      66,    69,    71,    73,    74,    75,    77,    78,    79,    80,
      81,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    56,    56,    72,    74,    75,    77,    80,
      84,    52,    91,    56,    54,    54,    54,    71,    75,    79,
      80,    82,    83,    87,     3,     6,    52,    80,    82,    83,
      90,     1,     3,    67,    68,     0,    48,    49,    23,    24,
      25,    26,     1,     3,    52,    69,    70,    90,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    76,    52,    50,
      51,     3,    82,    82,    80,    82,    69,    69,    80,     1,
      53,    97,    97,    59,    97,    97,    27,    55,     1,    27,
      57,    59,    80,    80,    73,    73,    73,    73,    27,    59,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      63,    63,    57,    57,    97,    57,    27,    49,    58,    80,
      80,    80,    65,     1,    68,    80,     1,    70,    97,    52,
      80,    80,    71,    96,    58,    72,    80,    58,    58,     1,
      55,    60,    98,    71,    97,    71,    71,    80,    96,    98,
      98,    58,    98,    80,    98
  };

  const signed char
  RSParserImpl::yyr1_[] =
  {
       0,    61,    62,    62,    63,    63,    64,    64,    64,    65,
      65,    66,    66,    67,    67,    67,    68,    68,    69,    69,
      70,    70,    70,    71,    71,    71,    72,    72,    72,    73,
      73,    74,    74,    75,    75,    75,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    77,    77,    77,
      77,    78,    78,    79,    79,    79,    79,    80,    80,    80,
      80,    80,    80,    81,    81,    81,    81,    81,    81,    82,
      82,    83,    84,    84,    84,    85,    85,    85,    86,    86,
      86,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      88,    88,    88,    88,    88,    88,    88,    89,    90,    91,
      91,    92,    93,    93,    94,    94,    95,    96,    96,    97,
      97,    98,    98
  };

  const signed char
  RSParserImpl::yyr2_[] =
  {
       0,     2,     1,     1,     1,     1,     2,     3,     3,     1,
       1,     4,     2,     1,     3,     3,     3,     2,     1,     1,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     5,     2,
       4,     1,     1,     3,     3,     3,     3,     1,     1,     1,
       1,     4,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     4,
       2,     7,     7,     8,    10,     8,     6,     1,     3,     1,
       1,     1,     1
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const RSParserImpl::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "LOCAL", "GLOBAL",
  "FUNCTION", "PREDICATE", "RADICAL", "INTEGER", "INTSET", "EMPTYSET",
  "PLUS", "MINUS", "MULTIPLY", "GREATER", "LESSER", "GREATER_OR_EQ",
  "LESSER_OR_EQ", "EQUAL", "NOTEQUAL", "FORALL", "EXISTS", "NOT",
  "EQUIVALENT", "IMPLICATION", "OR", "AND", "IN", "NOTIN", "SUBSET",
  "SUBSET_OR_EQ", "NOTSUBSET", "DECART", "UNION", "INTERSECTION",
  "SET_MINUS", "SYMMINUS", "BOOLEAN", "BIGPR", "SMALLPR", "FILTER", "CARD",
  "BOOL", "DEBOOL", "RED", "DECLARATIVE", "RECURSIVE", "IMPERATIVE",
  "ITERATE", "ASSIGN", "DEFINE", "STRUCT", "LP", "RP", "LC", "RC", "LS",
  "RS", "BAR", "COMMA", "SEMICOLON", "$accept", "expression",
  "no_declaration", "global_declaration", "logic_or_setexpr",
  "function_definition", "arguments", "declaration", "variable",
  "variable_pack", "logic", "logic_no_binary", "logic_all", "logic_par",
  "logic_predicates", "binary_predicate", "logic_unary", "quantifier",
  "logic_binary", "setexpr", "text_function", "setexpr_enum",
  "setexpr_enum_min2", "global_name", "literal", "identifier",
  "setexpr_binary", "setexpr_generators", "enumeration", "tuple",
  "boolean", "filter_expression", "declarative", "recursion", "imperative",
  "imp_blocks", "RPE", "RCE", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  RSParserImpl::yyrline_[] =
  {
       0,   270,   270,   271,   275,   276,   280,   281,   282,   286,
     287,   291,   292,   297,   298,   299,   302,   303,   307,   308,
     311,   312,   313,   318,   319,   320,   323,   324,   325,   328,
     329,   332,   333,   337,   338,   339,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   351,   352,   356,   357,   358,
     359,   362,   363,   367,   368,   369,   370,   376,   377,   378,
     379,   380,   381,   384,   385,   386,   387,   388,   389,   392,
     393,   396,   400,   401,   402,   406,   407,   408,   412,   413,
     414,   418,   419,   420,   421,   422,   423,   424,   425,   426,
     430,   431,   432,   433,   434,   435,   436,   439,   442,   445,
     446,   449,   453,   454,   457,   458,   461,   464,   465,   471,
     472,   475,   476
  };

  void
  RSParserImpl::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  RSParserImpl::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  RSParserImpl::symbol_kind_type
  RSParserImpl::yytranslate_ (int t)
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60
    };
    // Last valid token kind.
    const int code_max = 315;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return YY_CAST (symbol_kind_type, translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 15 "RSParserImpl.y"
} } } // ccl::rslang::detail
#line 1646 "RSParserImpl.cpp"

#line 483 "RSParserImpl.y"


#ifdef _MSC_VER
  #pragma warning( pop )
#endif

#ifdef __clang__
  #pragma clang diagnostic pop
#endif

#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic pop
#endif
