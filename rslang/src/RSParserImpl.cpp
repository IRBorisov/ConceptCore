// A Bison parser, made by GNU Bison 3.3.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2019 Free Software Foundation, Inc.

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

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.

// //                    "%code top" blocks.


#ifdef _MSC_VER
	#pragma warning( push )
	#pragma warning( disable : 26434 26438 26440 26446 26447 26448 26456 )
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



// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

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
      yystack_print_ ();                \
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


namespace ccl { namespace rslang { namespace parse {


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


  /// Build a parser object.
  RSParserImpl::RSParserImpl (ParserState* state_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      state (state_yyarg)
  {}

  RSParserImpl::~RSParserImpl ()
  {}

  RSParserImpl::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | Symbol types.  |
  `---------------*/

  // basic_symbol.
#if 201103L <= YY_CPLUSPLUS
  template <typename Base>
  RSParserImpl::basic_symbol<Base>::basic_symbol (basic_symbol&& that)
    : Base (std::move (that))
    , value (std::move (that.value))
  {}
#endif

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
  bool
  RSParserImpl::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  void
  RSParserImpl::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
  }

  // by_type.
  RSParserImpl::by_type::by_type ()
    : type (empty_symbol)
  {}

#if 201103L <= YY_CPLUSPLUS
  RSParserImpl::by_type::by_type (by_type&& that)
    : type (that.type)
  {
    that.clear ();
  }
#endif

  RSParserImpl::by_type::by_type (const by_type& that)
    : type (that.type)
  {}

  RSParserImpl::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  void
  RSParserImpl::by_type::clear ()
  {
    type = empty_symbol;
  }

  void
  RSParserImpl::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  int
  RSParserImpl::by_type::type_get () const YY_NOEXCEPT
  {
    return type;
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

  RSParserImpl::symbol_number_type
  RSParserImpl::by_state::type_get () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
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
    that.type = empty_symbol;
  }

#if YY_CPLUSPLUS < 201103L
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
    YYUSE (yysym.type_get ());
  }

#if YYDEBUG
  template <typename Base>
  void
  RSParserImpl::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
#if defined __GNUC__ && ! defined __clang__ && ! defined __ICC && __GNUC__ * 100 + __GNUC_MINOR__ <= 408
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
#endif
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " (";
    YYUSE (yytype);
    yyo << ')';
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
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
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
    // State.
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
    YYCDEBUG << "Entering state " << yystack_[0].state << '\n';

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, state));
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

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

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
    yypush_ ("Shifting", yyn, YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
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
  case 3:

    { state->FinalizeASTExpression(yystack_[0].value); }

    break;

  case 6:

    { state->FinalizeASTCstNoExpr(yystack_[1].value, yystack_[0].value); }

    break;

  case 7:

    { state->FinalizeASTCstExpr(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 8:

    { state->FinalizeASTCstExpr(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 9:

    { state->FinalizeASTFunction(yystack_[5].value, yystack_[4].value, yystack_[2].value, yystack_[0].value); }

    break;

  case 10:

    { state->OnError(ParseEID::expectedDeclaration); YYABORT; }

    break;

  case 11:

    { yylhs.value = AddNode(TokenID::NT_ARG_TYPES_ENUM, yystack_[0].value); }

    break;

  case 12:

    { yylhs.value = Enumeration(TokenID::NT_ARG_TYPES_ENUM, yystack_[2].value, yystack_[0].value); }

    break;

  case 13:

    { state->OnError(ParseEID::expectedLocal); YYABORT; }

    break;

  case 14:

    { yylhs.value = AddNode(TokenID::NT_ARG_DECL, yystack_[2].value, yystack_[0].value); }

    break;

  case 15:

    { state->OnError(ParseEID::expectedDeclaration); YYABORT; }

    break;

  case 24:

    { yylhs.value = RemoveBrackets(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 25:

    { yylhs.value = RemoveBrackets(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 26:

    { yylhs.value = UnaryOperation(yystack_[1].value, yystack_[0].value);}

    break;

  case 27:

    { yylhs.value = Quantifier(yystack_[4].value, yystack_[3].value, yystack_[1].value, yystack_[0].value);}

    break;

  case 28:

    { state->OnError(ParseEID::invalidQuantifier); YYABORT; }

    break;

  case 29:

    { yylhs.value = FunctionCall(yystack_[3].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 30:

    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 31:

    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 32:

    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 33:

    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 34:

    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 37:

    { state->OnError(ParseEID::invalidQuantifier); YYABORT; }

    break;

  case 38:

    { yylhs.value = Enumeration(TokenID::NT_ENUM_DECL, yystack_[2].value, yystack_[0].value); }

    break;

  case 39:

    { state->OnError(ParseEID::expectedLocal); YYABORT; }

    break;

  case 44:

    { yylhs.value = FunctionCall(yystack_[3].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 45:

    { yylhs.value = TextOperator(yystack_[3].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 46:

    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 47:

    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 48:

    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 49:

    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 50:

    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 51:

    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 52:

    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 53:

    { yylhs.value = Decartian(yystack_[2].value, yystack_[1].value, yystack_[0].value);}

    break;

  case 54:

    { yylhs.value = RemoveBrackets(yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 58:

    { yylhs.value = FilterCall(yystack_[6].value, yystack_[4].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 62:

    { yylhs.value = ReplaceBrackets(TokenID::NT_ENUMERATION, yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 63:

    { yylhs.value = ReplaceBrackets(TokenID::NT_TUPLE, yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 64:

    { yylhs.value = TextOperator(yystack_[3].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 65:

    { yylhs.value = UnaryOperation(yystack_[1].value, yystack_[0].value); }

    break;

  case 66:

    { yylhs.value = AddNode(TokenID::INTERRUPT, yystack_[0].value); }

    break;

  case 68:

    { yylhs.value = Enumeration(TokenID::INTERRUPT, yystack_[2].value, yystack_[0].value); }

    break;

  case 69:

    { yylhs.value = TermDeclaration(yystack_[6].value, yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 70:

    { yylhs.value = TermDeclaration(yystack_[7].value, yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 71:

    { yylhs.value = FullRecursion(yystack_[9].value, yystack_[7].value, yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 72:

    { yylhs.value = ShortRecursion(yystack_[7].value, yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 73:

    { yylhs.value = Imperative(yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 74:

    { yylhs.value = AddNode(TokenID::INTERRUPT, yystack_[0].value); }

    break;

  case 75:

    { yylhs.value = Enumeration(TokenID::INTERRUPT, yystack_[2].value, yystack_[0].value); }

    break;

  case 76:

    { yylhs.value = AddNode(TokenID::NT_IMP_DECLARE, yystack_[2].value, yystack_[0].value); }

    break;

  case 77:

    { yylhs.value = AddNode(TokenID::NT_IMP_ASSIGN, yystack_[2].value, yystack_[0].value); }

    break;

  case 78:

    { yylhs.value = AddNode(TokenID::NT_IMP_LOGIC, yystack_[0].value); }

    break;

  case 80:

    { yylhs.value = ReplaceBrackets(TokenID::NT_TUPLE_DECL, yystack_[2].value, yystack_[1].value, yystack_[0].value); }

    break;

  case 81:

    { yylhs.value = Enumeration(TokenID::INTERRUPT, yystack_[2].value, yystack_[0].value); }

    break;

  case 82:

    { state->OnError(ParseEID::expectedLocal); YYABORT; }

    break;

  case 115:

    { state->OnError(ParseEID::missingParanthesis); YYABORT; }

    break;

  case 117:

    { state->OnError(ParseEID::missingCurlyBrace); YYABORT; }

    break;



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
      YY_STACK_PRINT ();

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
        error (yysyntax_error_ (yystack_[0].state, yyla));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
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
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
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


      // Shift the error token.
      error_token.state = yyn;
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

  // Generate an error message.
  std::string
  RSParserImpl::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

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
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

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
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char RSParserImpl::yypact_ninf_ = -60;

  const signed char RSParserImpl::yytable_ninf_ = -85;

  const short
  RSParserImpl::yypact_[] =
  {
     327,   -60,   -20,   -31,   -19,   -60,   -60,   -60,   -60,   -60,
     -60,   379,    52,   -60,   -60,     3,   -60,   -60,   -60,   -60,
     -35,     9,    55,   379,   451,    47,   -60,   -60,   144,    91,
     -60,   -60,   -60,   -60,   588,   -60,   -60,   -60,   -60,   -60,
     -60,   -60,   -60,    58,   -60,   -60,   -60,    29,    98,   379,
     503,   503,   503,   -60,   -60,   -60,   -60,   -60,   588,   -60,
     503,   -60,   503,    31,    31,   503,   -60,    20,    54,   588,
     107,   105,     5,   141,   -60,   503,   227,   -47,   -60,   -60,
     379,   379,   379,   379,   503,   503,   503,   -60,   -60,   -60,
     -60,   -60,   -60,   -60,   -60,   -60,   -60,   -60,   503,   503,
     503,   503,   503,   503,   123,   -60,   -60,    30,     8,   -60,
     -60,   503,   -60,   227,   106,   110,   241,   143,    31,   160,
     138,   119,   -60,   -60,   -60,   -60,   -60,   503,   -60,   503,
     -60,   122,    65,   173,   -60,   171,   171,    61,   -60,   -60,
     -60,   -60,   -60,   227,   109,   -60,   -60,    17,   149,   503,
      35,   241,   -60,   -60,   -60,   157,   503,   503,   431,   227,
     526,   -60,    21,   163,   -60,   -60,    53,   275,   -60,   -60,
     -60,   503,   540,   553,   124,     6,    14,   -60,   379,   -60,
     503,   379,   220,   -60,   -60,   -60,   -60,   241,   379,   379,
     503,   503,   -60,   -60,   431,   -60,    16,   227,   -60,   -60,
     -60,   -60,    16,   166,   200,   227,   227,   -60,   -60,   -60,
     503,   -60,   236,   -60
  };

  const unsigned char
  RSParserImpl::yydefact_[] =
  {
       0,    91,    87,    85,    86,    89,    92,    94,    93,    95,
      96,     0,     0,   100,   101,     0,   102,    97,    98,    99,
       0,     0,     0,     0,     0,     0,     3,     2,    19,     0,
      20,    17,    16,    18,     5,    42,    43,    55,    56,    57,
      59,    61,    60,    88,    90,    41,    40,     0,     0,     6,
       0,     0,     0,    87,    26,    23,    22,    21,     0,    88,
       0,    65,     0,     0,     0,     0,    19,     0,     0,    66,
       0,     0,     0,    91,    86,     0,    66,     0,    67,     1,
       0,     0,     0,     0,     0,     0,     0,   110,   111,   112,
     113,   109,   108,   103,   104,   105,   106,   107,     0,     0,
       0,     0,     0,     0,     0,    28,    79,     0,     0,    36,
      35,     0,     8,     7,     0,     0,     0,     0,     0,     0,
       0,     0,   115,   114,    25,    24,    54,     0,    63,     0,
      62,    31,    32,    33,    34,    46,    47,    48,    53,    49,
      52,    50,    51,    30,     0,    37,    83,     0,     0,     0,
       0,     0,    44,    29,    64,     0,     0,     0,     0,    68,
       0,    10,     0,     0,    11,    80,     0,     0,    39,    38,
      45,     0,     0,     0,    91,    19,     0,    74,     0,    15,
       0,     0,     0,    82,    84,    81,    27,     0,     0,     0,
       0,     0,   117,   116,     0,    73,     0,    14,     9,    13,
      12,    58,     0,    19,     0,    77,    76,    75,    69,    70,
       0,    72,     0,    71
  };

  const short
  RSParserImpl::yypgoto_[] =
  {
     -60,   -60,   -48,   -60,   -60,    43,     4,   114,    59,    -8,
      -6,    -9,   218,    93,   -60,     0,    -7,   -60,   -60,   -60,
     232,   121,    -3,   -60,   -60,   -60,    51,   -60,   -37,    80,
      84,   251,   -60,   -60,   -60,   -60,   -60,   -60,   -59,   -26
  };

  const short
  RSParserImpl::yydefgoto_[] =
  {
      -1,    25,    26,    27,   163,   164,    66,    29,    54,    30,
      31,    32,    33,   108,   109,    58,    35,    36,    37,    38,
      39,    71,    78,    40,    41,    42,   176,   177,   146,   147,
     148,    59,    44,    45,    46,    47,    48,   103,   124,   195
  };

  const short
  RSParserImpl::yytable_[] =
  {
      34,   112,    57,    55,    28,    56,   122,   -78,   130,   125,
     110,   126,   127,   128,    67,   192,    70,   192,   122,    63,
      72,   122,   179,    69,    76,    51,   119,   120,    49,    50,
     105,   145,   106,   106,   106,   149,   168,    52,   106,   -19,
     -19,   -19,   -19,   -16,   -16,   -16,   -16,    79,   180,    34,
     113,    76,    76,    28,   183,   122,   106,   154,   123,    62,
     116,   -78,    76,    64,   -67,   121,   -78,   150,    70,   193,
     123,   193,    72,   123,   194,    76,   -84,   -18,   -18,   -18,
     -18,   107,   118,   118,   135,   136,   137,   107,   165,    12,
      82,    83,   170,    98,    99,   100,   101,   102,   138,   139,
     140,   141,   142,   143,    60,   118,   104,   123,   122,    65,
     161,   151,   162,   110,    80,    81,    82,    83,   -42,   -42,
     -42,   -42,   -42,   -42,   -42,   -42,   -42,   159,   201,   160,
      84,    85,    86,   198,   -42,   -42,   -42,   -42,   -42,   -42,
     -42,   -42,   -42,   -42,    -4,    77,    81,    82,    83,   167,
     111,    98,    99,   100,   101,   102,   172,   173,    57,    55,
     123,    56,   175,   152,   127,   127,   -42,   153,   129,   127,
     208,   187,   114,   115,   190,   191,   209,   158,   211,   144,
     197,    34,   196,   117,    86,    28,   213,   156,   157,   204,
     205,   206,   202,   203,   131,   132,   133,   134,   175,    83,
     155,   192,   127,    98,    99,   100,   101,   102,   166,   171,
     212,    84,    85,    86,    87,    88,    89,    90,    91,    92,
     181,   199,   182,   162,   210,   200,   186,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   192,    84,    85,
      86,    68,   122,   169,    61,   207,   184,    84,    85,    86,
     185,    43,    84,    85,    86,   193,     0,     0,     0,    98,
      99,   100,   101,   102,     0,     0,     0,     0,    98,    99,
     100,   101,   102,    98,    99,   100,   101,   102,     1,    53,
       3,     4,     5,     6,     7,     8,    84,    85,    86,     0,
       0,   193,     0,     0,   123,     9,    10,    11,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    98,    99,   100,
     101,   102,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,     0,     0,     0,     0,    23,     0,    24,
       1,     2,     3,     4,     5,     6,     7,     8,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     9,    10,    11,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,     0,     0,     0,     0,    23,
       0,    24,     1,    53,     3,     4,     5,     6,     7,     8,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     9,
      10,    11,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,     0,     0,     0,
       0,    23,     0,    24,   174,    53,     3,     4,     5,     6,
       7,     8,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     9,    10,    11,    73,    53,     3,    74,     5,     6,
       7,     8,     0,     0,     0,     0,     0,     0,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,     0,
       0,     0,     0,    23,     0,    24,     0,     0,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,     0,
       0,     0,     0,    75,     0,    24,     1,    53,     3,    74,
       5,     6,     7,     8,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    84,    85,    86,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    84,    85,    86,     0,    75,     0,    24,    98,    99,
     100,   101,   102,     0,    84,    85,    86,     0,     0,     0,
       0,     0,    98,    99,   100,   101,   102,     0,     0,     0,
       0,     0,     0,     0,   178,    98,    99,   100,   101,   102,
       0,     0,     0,     0,     0,     0,     0,     0,   188,    84,
      85,    86,    87,    88,    89,    90,    91,    92,     0,     0,
       0,   189,     0,     0,     0,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102
  };

  const short
  RSParserImpl::yycheck_[] =
  {
       0,    49,    11,    11,     0,    11,     1,     1,    55,    68,
      47,    70,    59,    72,    23,     1,    23,     1,     1,    54,
      23,     1,     1,    23,    24,    56,    63,    64,    48,    49,
       1,     1,     3,     3,     3,    27,     1,    56,     3,    23,
      24,    25,    26,    23,    24,    25,    26,     0,    27,    49,
      50,    51,    52,    49,     1,     1,     3,   116,    53,    56,
      60,    55,    62,    54,    59,    65,    60,    59,    75,    55,
      53,    55,    75,    53,    60,    75,    59,    23,    24,    25,
      26,    52,    52,    52,    84,    85,    86,    52,   147,    37,
      25,    26,   151,    32,    33,    34,    35,    36,    98,    99,
     100,   101,   102,   103,    52,    52,    48,    53,     1,    54,
       1,   111,     3,   150,    23,    24,    25,    26,    11,    12,
      13,    14,    15,    16,    17,    18,    19,   127,   187,   129,
      11,    12,    13,   181,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    24,    24,    25,    26,   149,
      52,    32,    33,    34,    35,    36,   156,   157,   167,   167,
      53,   167,   158,    57,    59,    59,    59,    57,    27,    59,
     196,   171,    51,    52,    50,    51,   202,    58,   204,    56,
     180,   181,   178,    62,    13,   181,   212,    27,    50,   189,
     190,   191,   188,   189,    80,    81,    82,    83,   194,    26,
      57,     1,    59,    32,    33,    34,    35,    36,    59,    52,
     210,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      57,     1,    59,     3,    58,   182,   167,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     1,    11,    12,
      13,    23,     1,   150,    12,   194,   166,    11,    12,    13,
     166,     0,    11,    12,    13,    55,    -1,    -1,    -1,    32,
      33,    34,    35,    36,    -1,    -1,    -1,    -1,    32,    33,
      34,    35,    36,    32,    33,    34,    35,    36,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    -1,
      -1,    55,    -1,    -1,    53,    20,    21,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    -1,    -1,    -1,    -1,    52,    -1,    54,
       3,     4,     5,     6,     7,     8,     9,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,    21,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,    52,
      -1,    54,     3,     4,     5,     6,     7,     8,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    -1,    -1,    -1,
      -1,    52,    -1,    54,     3,     4,     5,     6,     7,     8,
       9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    20,    21,    22,     3,     4,     5,     6,     7,     8,
       9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    -1,
      -1,    -1,    -1,    52,    -1,    54,    -1,    -1,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    -1,
      -1,    -1,    -1,    52,    -1,    54,     3,     4,     5,     6,
       7,     8,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,    12,    13,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    11,    12,    13,    -1,    52,    -1,    54,    32,    33,
      34,    35,    36,    -1,    11,    12,    13,    -1,    -1,    -1,
      -1,    -1,    32,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    58,    32,    33,    34,    35,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    -1,    -1,
      -1,    58,    -1,    -1,    -1,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36
  };

  const unsigned char
  RSParserImpl::yystos_[] =
  {
       0,     3,     4,     5,     6,     7,     8,     9,    10,    20,
      21,    22,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    52,    54,    62,    63,    64,    67,    68,
      70,    71,    72,    73,    76,    77,    78,    79,    80,    81,
      84,    85,    86,    92,    93,    94,    95,    96,    97,    48,
      49,    56,    56,     4,    69,    70,    71,    72,    76,    92,
      52,    81,    56,    54,    54,    54,    67,    72,    73,    76,
      77,    82,    83,     3,     6,    52,    76,    82,    83,     0,
      23,    24,    25,    26,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    98,    48,     1,     3,    52,    74,    75,
      89,    52,    63,    76,    82,    82,    76,    82,    52,    89,
      89,    76,     1,    53,    99,    99,    99,    59,    99,    27,
      55,    68,    68,    68,    68,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    56,     1,    89,    90,    91,    27,
      59,    76,    57,    57,    99,    57,    27,    50,    58,    76,
      76,     1,     3,    65,    66,    99,    59,    76,     1,    74,
      99,    52,    76,    76,     3,    67,    87,    88,    58,     1,
      27,    57,    59,     1,    90,    91,    69,    76,    58,    58,
      50,    51,     1,    55,    60,   100,    67,    76,    63,     1,
      66,    99,    67,    67,    76,    76,    76,    87,   100,   100,
      58,   100,    76,   100
  };

  const unsigned char
  RSParserImpl::yyr1_[] =
  {
       0,    61,    62,    62,    63,    63,    64,    64,    64,    64,
      64,    65,    65,    65,    66,    66,    67,    67,    67,    68,
      68,    69,    69,    69,    70,    70,    71,    71,    71,    71,
      72,    73,    73,    73,    73,    74,    74,    74,    75,    75,
      76,    76,    76,    76,    76,    76,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    78,    78,    78,    78,    78,
      78,    78,    79,    80,    81,    81,    82,    82,    83,    84,
      84,    85,    85,    86,    87,    87,    88,    88,    88,    89,
      89,    90,    90,    91,    91,    92,    92,    93,    93,    93,
      94,    94,    95,    95,    95,    96,    96,    97,    97,    97,
      97,    97,    97,    98,    98,    98,    98,    98,    98,    98,
      98,    98,    98,    98,    99,    99,   100,   100
  };

  const unsigned char
  RSParserImpl::yyr2_[] =
  {
       0,     2,     1,     1,     1,     1,     2,     3,     3,     6,
       4,     1,     3,     3,     3,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     2,     5,     2,     4,
       3,     3,     3,     3,     3,     1,     1,     2,     3,     3,
       1,     1,     1,     1,     4,     4,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     1,     1,     7,     1,
       1,     1,     3,     3,     4,     2,     1,     1,     3,     7,
       8,    10,     8,     6,     1,     3,     3,     3,     1,     1,
       3,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const RSParserImpl::yytname_[] =
  {
  "$end", "error", "$undefined", "LOCAL", "GLOBAL", "FUNCTION",
  "PREDICATE", "RADICAL", "INTEGER", "INFINITY", "EMPTYSET", "PLUS",
  "MINUS", "MULTIPLY", "GREATER", "LESSER", "GREATER_OR_EQ",
  "LESSER_OR_EQ", "EQUAL", "NOTEQUAL", "FORALL", "EXISTS", "NOT",
  "EQUIVALENT", "IMPLICATION", "OR", "AND", "IN", "NOTIN", "SUBSET",
  "SUBOR_EQ", "NOTSUBSET", "DECART", "UNION", "INTERSECTION", "SET_MINUS",
  "SYMMINUS", "BOOLEAN", "BIGPR", "SMALLPR", "FILTER", "CARD", "BOOL",
  "DEBOOL", "RED", "DECLARATIVE", "RECURSIVE", "IMPERATIVE", "DEFINE",
  "STRUCT", "ASSIGN", "ITERATE", "LP", "RP", "LC", "RC", "LS", "RS", "BAR",
  "COMMA", "SEMICOLON", "$accept", "expression", "term_or_logic",
  "global_declaration", "arg_declaration", "declaration", "logic",
  "logic_all", "logic_no_binary", "logic_par", "logic_unary",
  "logic_predicates", "logic_binary", "quant_var", "quant_var_enum",
  "term", "binary_operation", "typed_constructors", "enumeration", "tuple",
  "boolean", "term_enum", "term_enum_min2", "declarative", "recursion",
  "imperative", "imp_blocks", "imp_block", "variable", "var_enum",
  "var_all", "function_name", "global_id", "identifier", "literal",
  "quantifier", "operation_name", "binary_predicate", "RPE", "RCE", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned short
  RSParserImpl::yyrline_[] =
  {
       0,   148,   148,   149,   152,   153,   157,   158,   159,   160,
     162,   164,   165,   166,   168,   169,   173,   174,   175,   177,
     178,   180,   181,   182,   184,   185,   187,   188,   190,   191,
     193,   195,   196,   197,   198,   201,   202,   203,   205,   206,
     210,   211,   212,   213,   214,   215,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   229,   230,   231,   232,   233,
     234,   235,   237,   239,   241,   242,   244,   245,   247,   249,
     250,   253,   255,   258,   260,   261,   263,   264,   265,   267,
     268,   270,   271,   273,   274,   278,   279,   281,   282,   283,
     285,   286,   288,   289,   290,   293,   294,   297,   298,   299,
     300,   301,   302,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   318,   319,   321,   322
  };

  // Print the state stack on the debug stream.
  void
  RSParserImpl::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << '\n';
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  RSParserImpl::yy_reduce_print_ (int yyrule)
  {
    unsigned yylno = yyrline_[yyrule];
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

  RSParserImpl::token_number_type
  RSParserImpl::yytranslate_ (int t)
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60
    };
    const unsigned user_token_number_max_ = 315;
    const token_number_type undef_token_ = 2;

    if (static_cast<int> (t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} } } // ccl::rslang::parse




#ifdef _MSC_VER
	#pragma warning( pop )
#endif

#ifdef __clang__
	#pragma clang diagnostic pop
#endif

#if defined(__GNUC__) && !defined(__clang__)
	#pragma GCC diagnostic pop
#endif
