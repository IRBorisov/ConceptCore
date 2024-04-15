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
#line 16 "RSParserImpl.y" // lalr1.cc:423

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


#line 58 "RSParserImpl.cpp" // lalr1.cc:423




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

#line 11 "RSParserImpl.y" // lalr1.cc:510
namespace ccl { namespace rslang { namespace detail {
#line 141 "RSParserImpl.cpp" // lalr1.cc:510

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
#line 151 "RSParserImpl.y" // lalr1.cc:919
    { state->FinalizeExpression(yystack_[0].value); }
#line 609 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 4:
#line 152 "RSParserImpl.y" // lalr1.cc:919
    { state->FinalizeExpression(yystack_[0].value); }
#line 615 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 7:
#line 159 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = FunctionDeclaration(yystack_[3].value, yystack_[2].value, yystack_[0].value); }
#line 621 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 8:
#line 160 "RSParserImpl.y" // lalr1.cc:919
    { state->OnError(ParseEID::expectedDeclaration); YYABORT; }
#line 627 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 9:
#line 163 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = AddNode(TokenID::NT_ARGUMENTS, yystack_[0].value); }
#line 633 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 10:
#line 164 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = Enumeration(TokenID::NT_ARGUMENTS, yystack_[2].value, yystack_[0].value); }
#line 639 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 11:
#line 165 "RSParserImpl.y" // lalr1.cc:919
    { state->OnError(ParseEID::expectedLocal); YYABORT; }
#line 645 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 12:
#line 167 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = AddNode(TokenID::NT_ARG_DECL, yystack_[2].value, yystack_[0].value); }
#line 651 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 13:
#line 168 "RSParserImpl.y" // lalr1.cc:919
    { state->OnError(ParseEID::expectedDeclaration); YYABORT; }
#line 657 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 14:
#line 172 "RSParserImpl.y" // lalr1.cc:919
    { state->FinalizeCstEmpty(yystack_[1].value, yystack_[0].value); }
#line 663 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 15:
#line 173 "RSParserImpl.y" // lalr1.cc:919
    { state->FinalizeCstExpression(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 669 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 16:
#line 174 "RSParserImpl.y" // lalr1.cc:919
    { state->FinalizeCstExpression(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 675 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 17:
#line 175 "RSParserImpl.y" // lalr1.cc:919
    { state->FinalizeCstExpression(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 681 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 26:
#line 190 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = RemoveBrackets(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 687 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 27:
#line 191 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = RemoveBrackets(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 693 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 28:
#line 193 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = UnaryOperation(yystack_[1].value, yystack_[0].value);}
#line 699 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 29:
#line 195 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = Quantifier(yystack_[4].value, yystack_[3].value, yystack_[1].value, yystack_[0].value);}
#line 705 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 30:
#line 196 "RSParserImpl.y" // lalr1.cc:919
    { state->OnError(ParseEID::invalidQuantifier); YYABORT; }
#line 711 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 31:
#line 197 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = FunctionCall(yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 717 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 32:
#line 199 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 723 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 33:
#line 201 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 729 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 34:
#line 202 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 735 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 35:
#line 203 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 741 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 36:
#line 204 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 747 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 39:
#line 209 "RSParserImpl.y" // lalr1.cc:919
    { state->OnError(ParseEID::invalidQuantifier); YYABORT; }
#line 753 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 40:
#line 211 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = Enumeration(TokenID::NT_ENUM_DECL, yystack_[2].value, yystack_[0].value); }
#line 759 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 41:
#line 212 "RSParserImpl.y" // lalr1.cc:919
    { state->OnError(ParseEID::expectedLocal); YYABORT; }
#line 765 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 46:
#line 220 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = FunctionCall(yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 771 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 47:
#line 221 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = TextOperator(yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 777 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 48:
#line 224 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 783 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 49:
#line 225 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 789 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 50:
#line 226 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 795 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 51:
#line 227 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 801 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 52:
#line 228 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 807 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 53:
#line 229 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 813 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 54:
#line 230 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = BinaryOperation(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 819 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 55:
#line 231 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = Decartian(yystack_[2].value, yystack_[1].value, yystack_[0].value);}
#line 825 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 56:
#line 232 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = RemoveBrackets(yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 831 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 60:
#line 238 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = FilterCall(yystack_[6].value, yystack_[4].value, yystack_[1].value, yystack_[0].value); }
#line 837 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 64:
#line 243 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = ReplaceBrackets(TokenID::NT_ENUMERATION, yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 843 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 65:
#line 245 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = ReplaceBrackets(TokenID::NT_TUPLE, yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 849 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 66:
#line 247 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = TextOperator(yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 855 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 67:
#line 248 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = UnaryOperation(yystack_[1].value, yystack_[0].value); }
#line 861 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 68:
#line 250 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = AddNode(TokenID::INTERRUPT, yystack_[0].value); }
#line 867 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 70:
#line 253 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = Enumeration(TokenID::INTERRUPT, yystack_[2].value, yystack_[0].value); }
#line 873 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 71:
#line 255 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = TermDeclaration(yystack_[6].value, yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 879 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 72:
#line 257 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = TermDeclaration(yystack_[7].value, yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 885 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 73:
#line 260 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = FullRecursion(yystack_[9].value, yystack_[7].value, yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 891 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 74:
#line 262 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = ShortRecursion(yystack_[7].value, yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 897 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 75:
#line 264 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = Imperative(yystack_[5].value, yystack_[3].value, yystack_[1].value, yystack_[0].value); }
#line 903 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 76:
#line 266 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = AddNode(TokenID::INTERRUPT, yystack_[0].value); }
#line 909 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 77:
#line 267 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = Enumeration(TokenID::INTERRUPT, yystack_[2].value, yystack_[0].value); }
#line 915 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 78:
#line 269 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = AddNode(TokenID::NT_IMP_DECLARE, yystack_[2].value, yystack_[0].value); }
#line 921 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 79:
#line 270 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = AddNode(TokenID::NT_IMP_ASSIGN, yystack_[2].value, yystack_[0].value); }
#line 927 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 80:
#line 271 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = AddNode(TokenID::NT_IMP_LOGIC, yystack_[0].value); }
#line 933 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 82:
#line 274 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = ReplaceBrackets(TokenID::NT_TUPLE_DECL, yystack_[2].value, yystack_[1].value, yystack_[0].value); }
#line 939 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 83:
#line 276 "RSParserImpl.y" // lalr1.cc:919
    { yylhs.value = Enumeration(TokenID::INTERRUPT, yystack_[2].value, yystack_[0].value); }
#line 945 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 84:
#line 277 "RSParserImpl.y" // lalr1.cc:919
    { state->OnError(ParseEID::expectedLocal); YYABORT; }
#line 951 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 117:
#line 325 "RSParserImpl.y" // lalr1.cc:919
    { state->OnError(ParseEID::missingParanthesis); YYABORT; }
#line 957 "RSParserImpl.cpp" // lalr1.cc:919
    break;

  case 119:
#line 328 "RSParserImpl.y" // lalr1.cc:919
    { state->OnError(ParseEID::missingCurlyBrace); YYABORT; }
#line 963 "RSParserImpl.cpp" // lalr1.cc:919
    break;


#line 967 "RSParserImpl.cpp" // lalr1.cc:919
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


  const signed char RSParserImpl::yypact_ninf_ = -65;

  const signed char RSParserImpl::yytable_ninf_ = -87;

  const short
  RSParserImpl::yypact_[] =
  {
     236,   -65,    92,   -43,   -40,   -65,   -65,   -65,   -65,   -65,
     -65,   342,    37,   -65,   -65,   -37,   -65,   -65,   -65,   -65,
     -16,    21,    26,   342,   414,   143,    56,   -65,   -65,   -65,
     110,    90,   -65,   -65,   -65,   -65,   553,   -65,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,    83,   -65,   -65,   -65,    84,
      85,   342,   466,   466,   466,   -65,   -65,   -65,   -65,   -65,
     553,   -65,   466,   -65,   466,    93,    93,   466,   -65,    58,
     101,   553,   202,   102,    19,   146,   -65,   466,   215,     2,
     -65,   -65,    59,   103,   -65,   -65,   342,   342,   342,   342,
     466,   466,   466,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   -65,   -65,   466,   466,   466,   466,   466,   466,
     114,   -65,   -65,   100,    12,   -65,   -65,   466,   -65,   215,
     123,   195,   152,   203,    93,   163,   144,   489,   -65,   -65,
     -65,   -65,   -65,   466,   -65,   466,   -65,   -65,   466,   342,
     199,   125,   238,   185,   -65,   142,   142,   174,   -65,   -65,
     -65,   -65,   -65,   215,   -65,   -65,   -65,    20,   194,   466,
     127,   152,   -65,   -65,   -65,   207,   466,   466,   394,   215,
     503,   215,   -65,   -65,   -65,   -65,   131,   290,   -65,   -65,
     -65,   466,   516,   521,   106,     8,    10,   -65,   342,   -65,
     -65,   -65,   -65,   152,   342,   342,   466,   466,   -65,   -65,
     394,   -65,    11,   -65,    11,   208,    14,   215,   215,   -65,
     -65,   -65,   466,   -65,    87,   -65
  };

  const unsigned char
  RSParserImpl::yydefact_[] =
  {
       0,    93,    89,    87,    88,    91,    94,    96,    95,    97,
      98,     0,     0,   102,   103,     0,   104,    99,   100,   101,
       0,     0,     0,     0,     0,     0,     0,     3,     4,     2,
      21,     0,    22,    19,    18,    20,     6,    44,    45,    57,
      58,    59,    61,    63,    62,    90,    92,    43,    42,     0,
       0,    14,     0,     0,     0,    89,    28,    25,    24,    23,
       0,    90,     0,    67,     0,     0,     0,     0,    21,     0,
       0,    68,     0,     0,     0,    93,    88,     0,    68,     0,
      69,     8,     0,     0,     9,     1,     0,     0,     0,     0,
       0,     0,     0,   112,   113,   114,   115,   111,   110,   105,
     106,   107,   108,   109,     0,     0,     0,     0,     0,     0,
       0,    30,    81,     0,     0,    38,    37,     0,    16,    15,
       0,     0,     0,     0,     0,     0,     0,     0,   117,   116,
      27,    26,    56,     0,    65,     0,    64,    13,     0,     0,
       0,    33,    34,    35,    36,    48,    49,    50,    55,    51,
      54,    52,    53,    32,    17,    39,    85,     0,     0,     0,
       0,     0,    46,    31,    66,     0,     0,     0,     0,    70,
       0,    12,     7,    11,    10,    82,     0,     0,    41,    40,
      47,     0,     0,     0,    93,    21,     0,    76,     0,    84,
      86,    83,    29,     0,     0,     0,     0,     0,   119,   118,
       0,    75,     0,    60,     0,    21,     0,    79,    78,    77,
      71,    72,     0,    74,     0,    73
  };

  const short
  RSParserImpl::yypgoto_[] =
  {
     -65,   -65,   -44,   155,   -65,   128,   -65,     4,   136,    95,
      -8,    -6,    -9,   244,   109,   -65,     0,    -1,   -65,   -65,
     -65,   258,    94,    17,   -65,   -65,   -65,    71,   -65,   -48,
     108,   111,   285,   -65,   -65,   -65,   -65,   -65,   -65,   -64,
     -13
  };

  const short
  RSParserImpl::yydefgoto_[] =
  {
      -1,    26,    27,    28,    83,    84,    29,    68,    31,    56,
      32,    33,    34,    35,   114,   115,    60,    37,    38,    39,
      40,    41,    73,    80,    42,    43,    44,   186,   187,   156,
     157,   158,    61,    46,    47,    48,    49,    50,   109,   130,
     201
  };

  const short
  RSParserImpl::yytable_[] =
  {
      36,   116,    59,    57,    30,    58,   131,   118,   132,   -80,
     134,   198,   198,    53,    69,   198,    54,   125,   126,    64,
     128,   128,    72,    71,    78,    90,    91,    92,    93,    94,
      95,    96,    97,    98,   -21,   -21,   -21,   -21,    65,   159,
      74,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,    36,   119,    78,    78,    30,    85,   136,   164,   128,
     137,   133,   122,   -80,    78,   199,   199,   127,   -80,   199,
     200,   160,   129,   129,    12,    66,    72,    78,   -69,   -86,
      67,   -18,   -18,   -18,   -18,   111,   138,   112,   198,    62,
     145,   146,   147,   175,    74,   172,   112,   180,    90,    91,
      92,   155,   128,   112,   148,   149,   150,   151,   152,   153,
      -5,   129,   116,    86,    87,    88,    89,   161,    79,   104,
     105,   106,   107,   108,   -20,   -20,   -20,   -20,   178,   203,
     112,   110,   189,   169,   112,   170,   113,   117,   171,    36,
      51,    52,   199,    30,    81,   124,    82,   120,   121,    87,
      88,    89,   124,   128,   129,    92,   196,   197,   123,   177,
     139,   133,   140,    90,    91,    92,   182,   183,    59,    57,
      25,    58,   185,   135,   104,   105,   106,   107,   108,   113,
     162,   193,   133,   124,   104,   105,   106,   107,   108,   210,
     166,   211,   202,   213,   167,   206,   207,   208,   204,   205,
     173,   215,    82,   128,   185,   129,   104,   105,   106,   107,
     108,    89,   214,   -44,   -44,   -44,   -44,   -44,   -44,   -44,
     -44,   -44,   141,   142,   143,   144,    90,    91,    92,   -44,
     -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,     1,
       2,     3,     4,     5,     6,     7,     8,   104,   105,   106,
     107,   108,   163,   176,   133,   129,     9,    10,    11,   181,
     165,   -44,   133,    88,    89,   154,   212,    70,   174,   179,
      63,   209,   192,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,   190,    45,     0,   191,    23,     0,
      24,     0,    25,     1,    55,     3,     4,     5,     6,     7,
       8,    90,    91,    92,     0,     0,     0,     0,     0,     0,
       9,    10,    11,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   104,   105,   106,   107,   108,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,     0,     0,
       0,     0,    23,     0,    24,     1,    55,     3,     4,     5,
       6,     7,     8,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     9,    10,    11,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
       0,     0,     0,     0,    23,     0,    24,   184,    55,     3,
       4,     5,     6,     7,     8,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     9,    10,    11,    75,    55,     3,
      76,     5,     6,     7,     8,     0,     0,     0,     0,     0,
       0,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,     0,     0,     0,     0,    23,     0,    24,     0,
       0,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,     0,     0,     0,     0,    77,     0,    24,     1,
      55,     3,    76,     5,     6,     7,     8,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      90,    91,    92,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    90,    91,    92,     0,    77,     0,
      24,   104,   105,   106,   107,   108,     0,    90,    91,    92,
       0,     0,    90,    91,    92,   104,   105,   106,   107,   108,
       0,     0,     0,     0,     0,     0,     0,   168,   104,   105,
     106,   107,   108,   104,   105,   106,   107,   108,     0,     0,
       0,   188,     0,     0,    90,    91,    92,    93,    94,    95,
      96,    97,    98,     0,   194,     0,     0,     0,     0,   195,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108
  };

  const short
  RSParserImpl::yycheck_[] =
  {
       0,    49,    11,    11,     0,    11,    70,    51,    72,     1,
      74,     1,     1,    56,    23,     1,    56,    65,    66,    56,
       1,     1,    23,    23,    24,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    23,    24,    25,    26,    54,    27,
      23,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    51,    52,    53,    54,    51,     0,    55,   122,     1,
       1,    59,    62,    55,    64,    55,    55,    67,    60,    55,
      60,    59,    53,    53,    37,    54,    77,    77,    59,    59,
      54,    23,    24,    25,    26,     1,    27,     3,     1,    52,
      90,    91,    92,   157,    77,   139,     3,   161,    11,    12,
      13,     1,     1,     3,   104,   105,   106,   107,   108,   109,
       0,    53,   160,    23,    24,    25,    26,   117,    24,    32,
      33,    34,    35,    36,    23,    24,    25,    26,     1,   193,
       3,    48,     1,   133,     3,   135,    52,    52,   138,   139,
      48,    49,    55,   139,     1,    52,     3,    53,    54,    24,
      25,    26,    52,     1,    53,    13,    50,    51,    64,   159,
      57,    59,    59,    11,    12,    13,   166,   167,   177,   177,
      56,   177,   168,    27,    32,    33,    34,    35,    36,    52,
      57,   181,    59,    52,    32,    33,    34,    35,    36,   202,
      27,   204,   188,   206,    50,   195,   196,   197,   194,   195,
       1,   214,     3,     1,   200,    53,    32,    33,    34,    35,
      36,    26,   212,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    86,    87,    88,    89,    11,    12,    13,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     3,
       4,     5,     6,     7,     8,     9,    10,    32,    33,    34,
      35,    36,    57,    59,    59,    53,    20,    21,    22,    52,
      57,    59,    59,    25,    26,   110,    58,    23,   140,   160,
      12,   200,   177,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,   176,     0,    -1,   176,    52,    -1,
      54,    -1,    56,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    -1,    -1,    -1,    -1,    -1,    -1,
      20,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    -1,    -1,
      -1,    -1,    52,    -1,    54,     3,     4,     5,     6,     7,
       8,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    20,    21,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      -1,    -1,    -1,    -1,    52,    -1,    54,     3,     4,     5,
       6,     7,     8,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    20,    21,    22,     3,     4,     5,
       6,     7,     8,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    -1,    -1,    -1,    -1,    52,    -1,    54,    -1,
      -1,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    -1,    -1,    -1,    -1,    52,    -1,    54,     3,
       4,     5,     6,     7,     8,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      11,    12,    13,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    11,    12,    13,    -1,    52,    -1,
      54,    32,    33,    34,    35,    36,    -1,    11,    12,    13,
      -1,    -1,    11,    12,    13,    32,    33,    34,    35,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    32,    33,
      34,    35,    36,    32,    33,    34,    35,    36,    -1,    -1,
      -1,    58,    -1,    -1,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    -1,    58,    -1,    -1,    -1,    -1,    58,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36
  };

  const unsigned char
  RSParserImpl::yystos_[] =
  {
       0,     3,     4,     5,     6,     7,     8,     9,    10,    20,
      21,    22,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    52,    54,    56,    62,    63,    64,    67,
      68,    69,    71,    72,    73,    74,    77,    78,    79,    80,
      81,    82,    85,    86,    87,    93,    94,    95,    96,    97,
      98,    48,    49,    56,    56,     4,    70,    71,    72,    73,
      77,    93,    52,    82,    56,    54,    54,    54,    68,    73,
      74,    77,    78,    83,    84,     3,     6,    52,    77,    83,
      84,     1,     3,    65,    66,     0,    23,    24,    25,    26,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    99,
      48,     1,     3,    52,    75,    76,    90,    52,    63,    77,
      83,    83,    77,    83,    52,    90,    90,    77,     1,    53,
     100,   100,   100,    59,   100,    27,    55,     1,    27,    57,
      59,    69,    69,    69,    69,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    64,     1,    90,    91,    92,    27,
      59,    77,    57,    57,   100,    57,    27,    50,    58,    77,
      77,    77,    63,     1,    66,   100,    59,    77,     1,    75,
     100,    52,    77,    77,     3,    68,    88,    89,    58,     1,
      91,    92,    70,    77,    58,    58,    50,    51,     1,    55,
      60,   101,    68,   100,    68,    68,    77,    77,    77,    88,
     101,   101,    58,   101,    77,   101
  };

  const unsigned char
  RSParserImpl::yyr1_[] =
  {
       0,    61,    62,    62,    62,    63,    63,    64,    64,    65,
      65,    65,    66,    66,    67,    67,    67,    67,    68,    68,
      68,    69,    69,    70,    70,    70,    71,    71,    72,    72,
      72,    72,    73,    74,    74,    74,    74,    75,    75,    75,
      76,    76,    77,    77,    77,    77,    77,    77,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    79,    79,    79,
      79,    79,    79,    79,    80,    81,    82,    82,    83,    83,
      84,    85,    85,    86,    86,    87,    88,    88,    89,    89,
      89,    90,    90,    91,    91,    92,    92,    93,    93,    94,
      94,    94,    95,    95,    96,    96,    96,    97,    97,    98,
      98,    98,    98,    98,    98,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,   100,   100,   101,   101
  };

  const unsigned char
  RSParserImpl::yyr2_[] =
  {
       0,     2,     1,     1,     1,     1,     1,     4,     2,     1,
       3,     3,     3,     2,     2,     3,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     2,     5,
       2,     4,     3,     3,     3,     3,     3,     1,     1,     2,
       3,     3,     1,     1,     1,     1,     4,     4,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     1,     1,
       7,     1,     1,     1,     3,     3,     4,     2,     1,     1,
       3,     7,     8,    10,     8,     6,     1,     3,     3,     3,
       1,     1,     3,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1
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
  "function_decl", "arg_declaration", "declaration", "global_declaration",
  "logic", "logic_all", "logic_no_binary", "logic_par", "logic_unary",
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
       0,   150,   150,   151,   152,   155,   156,   159,   160,   163,
     164,   165,   167,   168,   172,   173,   174,   175,   179,   180,
     181,   183,   184,   186,   187,   188,   190,   191,   193,   194,
     196,   197,   199,   201,   202,   203,   204,   207,   208,   209,
     211,   212,   216,   217,   218,   219,   220,   221,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   235,   236,   237,
     238,   239,   240,   241,   243,   245,   247,   248,   250,   251,
     253,   255,   256,   259,   261,   264,   266,   267,   269,   270,
     271,   273,   274,   276,   277,   279,   280,   284,   285,   287,
     288,   289,   291,   292,   294,   295,   296,   299,   300,   303,
     304,   305,   306,   307,   308,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   324,   325,   327,   328
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

#line 11 "RSParserImpl.y" // lalr1.cc:1242
} } } // ccl::rslang::detail
#line 1629 "RSParserImpl.cpp" // lalr1.cc:1242
#line 331 "RSParserImpl.y" // lalr1.cc:1243


#ifdef _MSC_VER
  #pragma warning( pop )
#endif

#ifdef __clang__
  #pragma clang diagnostic pop
#endif

#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic pop
#endif
