// A Bison parser, made by GNU Bison 3.7.4.

// Skeleton interface for Bison LALR(1) parsers in C++

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


/**
 ** \file ../header/RSParserImpl.h
 ** Define the ccl::rslang::detail::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_YY_HEADER_RSPARSERIMPL_H_INCLUDED
# define YY_YY_HEADER_RSPARSERIMPL_H_INCLUDED
// "%code requires" blocks.
#line 48 "RSParserImpl.y"

#include "ccl/rslang/SyntaxTree.h"
#include "ccl/rslang/ParserState.hpp"

namespace ccl::rslang::detail {

#define YYSTYPE RawNode
struct ParserState;

int yylex(RawNode* yylval, ParserState* state);

RawNode AddNode(TokenID token, RawNode son);
RawNode AddNode(TokenID token, RawNode son1, RawNode son2);

RawNode RemoveBrackets(RawNode br1, RawNode operand, RawNode br2);
RawNode ReplaceBrackets(TokenID token, RawNode br1, RawNode argList, RawNode br2);
RawNode BinaryOperation(RawNode op1, RawNode operation, RawNode op2);
RawNode UnaryOperation(RawNode operation, RawNode operand);
RawNode Enumeration(TokenID token, RawNode el1, RawNode el2);

RawNode Decartian(
    RawNode op1,
    RawNode decartian,
    RawNode op2
);

RawNode Quantifier(
    RawNode quant,
    RawNode declaration,
    RawNode domain,
    RawNode predicate
);

RawNode FunctionDeclaration(
    RawNode start,
    RawNode argumentsDeclaration,
    RawNode expr
);

RawNode FunctionCall(
    RawNode function,
    RawNode args, RawNode
    rs
);

RawNode FilterCall(
    RawNode filter,
    RawNode params,
    RawNode argument,
    RawNode rp
);

RawNode TextOperator(
    RawNode operatorName,
    RawNode args,
    RawNode rp
);

RawNode TermDeclaration(
    RawNode lc,
    RawNode declaration,
    RawNode domain,
    RawNode predicate,
    RawNode rc);

RawNode FullRecursion(
    RawNode rec,
    RawNode declaration,
    RawNode domain,
    RawNode condition,
    RawNode iteration,
    RawNode rc
);

RawNode ShortRecursion(
    RawNode rec,
    RawNode declaration,
    RawNode domain,
    RawNode iteration,
    RawNode rc
);

RawNode Imperative(
    RawNode imp,
    RawNode value,
    RawNode actions,
    RawNode rc
);

} // namespace ccl::rslang::detail

#line 141 "../header/RSParserImpl.h"

# include <cassert>
# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif



#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

#line 15 "RSParserImpl.y"
namespace ccl { namespace rslang { namespace detail {
#line 271 "../header/RSParserImpl.h"




  /// A Bison parser.
  class RSParserImpl
  {
  public:
#ifndef YYSTYPE
    /// Symbol semantic values.
    typedef int semantic_type;
#else
    typedef YYSTYPE semantic_type;
#endif

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const std::string& m)
        : std::runtime_error (m)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        RST_YYEMPTY = -2,
    RST_YYEOF = 0,                 // "end of file"
    RST_YYerror = 256,             // error
    RST_YYUNDEF = 257,             // "invalid token"
    RST_LOCAL = 258,               // LOCAL
    RST_GLOBAL = 259,              // GLOBAL
    RST_FUNCTION = 260,            // FUNCTION
    RST_PREDICATE = 261,           // PREDICATE
    RST_RADICAL = 262,             // RADICAL
    RST_INTEGER = 263,             // INTEGER
    RST_INTSET = 264,              // INTSET
    RST_EMPTYSET = 265,            // EMPTYSET
    RST_PLUS = 266,                // PLUS
    RST_MINUS = 267,               // MINUS
    RST_MULTIPLY = 268,            // MULTIPLY
    RST_GREATER = 269,             // GREATER
    RST_LESSER = 270,              // LESSER
    RST_GREATER_OR_EQ = 271,       // GREATER_OR_EQ
    RST_LESSER_OR_EQ = 272,        // LESSER_OR_EQ
    RST_EQUAL = 273,               // EQUAL
    RST_NOTEQUAL = 274,            // NOTEQUAL
    RST_FORALL = 275,              // FORALL
    RST_EXISTS = 276,              // EXISTS
    RST_NOT = 277,                 // NOT
    RST_EQUIVALENT = 278,          // EQUIVALENT
    RST_IMPLICATION = 279,         // IMPLICATION
    RST_OR = 280,                  // OR
    RST_AND = 281,                 // AND
    RST_IN = 282,                  // IN
    RST_NOTIN = 283,               // NOTIN
    RST_SUBSET = 284,              // SUBSET
    RST_SUBSET_OR_EQ = 285,        // SUBSET_OR_EQ
    RST_NOTSUBSET = 286,           // NOTSUBSET
    RST_DECART = 287,              // DECART
    RST_UNION = 288,               // UNION
    RST_INTERSECTION = 289,        // INTERSECTION
    RST_SET_MINUS = 290,           // SET_MINUS
    RST_SYMMINUS = 291,            // SYMMINUS
    RST_BOOLEAN = 292,             // BOOLEAN
    RST_BIGPR = 293,               // BIGPR
    RST_SMALLPR = 294,             // SMALLPR
    RST_FILTER = 295,              // FILTER
    RST_CARD = 296,                // CARD
    RST_BOOL = 297,                // BOOL
    RST_DEBOOL = 298,              // DEBOOL
    RST_RED = 299,                 // RED
    RST_DECLARATIVE = 300,         // DECLARATIVE
    RST_RECURSIVE = 301,           // RECURSIVE
    RST_IMPERATIVE = 302,          // IMPERATIVE
    RST_DEFINE = 303,              // DEFINE
    RST_STRUCT = 304,              // STRUCT
    RST_ASSIGN = 305,              // ASSIGN
    RST_ITERATE = 306,             // ITERATE
    RST_LP = 307,                  // LP
    RST_RP = 308,                  // RP
    RST_LC = 309,                  // LC
    RST_RC = 310,                  // RC
    RST_LS = 311,                  // LS
    RST_RS = 312,                  // RS
    RST_BAR = 313,                 // BAR
    RST_COMMA = 314,               // COMMA
    RST_SEMICOLON = 315            // SEMICOLON
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::yytokentype token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 61, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_LOCAL = 3,                             // LOCAL
        S_GLOBAL = 4,                            // GLOBAL
        S_FUNCTION = 5,                          // FUNCTION
        S_PREDICATE = 6,                         // PREDICATE
        S_RADICAL = 7,                           // RADICAL
        S_INTEGER = 8,                           // INTEGER
        S_INTSET = 9,                            // INTSET
        S_EMPTYSET = 10,                         // EMPTYSET
        S_PLUS = 11,                             // PLUS
        S_MINUS = 12,                            // MINUS
        S_MULTIPLY = 13,                         // MULTIPLY
        S_GREATER = 14,                          // GREATER
        S_LESSER = 15,                           // LESSER
        S_GREATER_OR_EQ = 16,                    // GREATER_OR_EQ
        S_LESSER_OR_EQ = 17,                     // LESSER_OR_EQ
        S_EQUAL = 18,                            // EQUAL
        S_NOTEQUAL = 19,                         // NOTEQUAL
        S_FORALL = 20,                           // FORALL
        S_EXISTS = 21,                           // EXISTS
        S_NOT = 22,                              // NOT
        S_EQUIVALENT = 23,                       // EQUIVALENT
        S_IMPLICATION = 24,                      // IMPLICATION
        S_OR = 25,                               // OR
        S_AND = 26,                              // AND
        S_IN = 27,                               // IN
        S_NOTIN = 28,                            // NOTIN
        S_SUBSET = 29,                           // SUBSET
        S_SUBSET_OR_EQ = 30,                     // SUBSET_OR_EQ
        S_NOTSUBSET = 31,                        // NOTSUBSET
        S_DECART = 32,                           // DECART
        S_UNION = 33,                            // UNION
        S_INTERSECTION = 34,                     // INTERSECTION
        S_SET_MINUS = 35,                        // SET_MINUS
        S_SYMMINUS = 36,                         // SYMMINUS
        S_BOOLEAN = 37,                          // BOOLEAN
        S_BIGPR = 38,                            // BIGPR
        S_SMALLPR = 39,                          // SMALLPR
        S_FILTER = 40,                           // FILTER
        S_CARD = 41,                             // CARD
        S_BOOL = 42,                             // BOOL
        S_DEBOOL = 43,                           // DEBOOL
        S_RED = 44,                              // RED
        S_DECLARATIVE = 45,                      // DECLARATIVE
        S_RECURSIVE = 46,                        // RECURSIVE
        S_IMPERATIVE = 47,                       // IMPERATIVE
        S_DEFINE = 48,                           // DEFINE
        S_STRUCT = 49,                           // STRUCT
        S_ASSIGN = 50,                           // ASSIGN
        S_ITERATE = 51,                          // ITERATE
        S_LP = 52,                               // LP
        S_RP = 53,                               // RP
        S_LC = 54,                               // LC
        S_RC = 55,                               // RC
        S_LS = 56,                               // LS
        S_RS = 57,                               // RS
        S_BAR = 58,                              // BAR
        S_COMMA = 59,                            // COMMA
        S_SEMICOLON = 60,                        // SEMICOLON
        S_YYACCEPT = 61,                         // $accept
        S_expression = 62,                       // expression
        S_global_declaration = 63,               // global_declaration
        S_logic_or_setexpr = 64,                 // logic_or_setexpr
        S_function_definition = 65,              // function_definition
        S_arguments = 66,                        // arguments
        S_declaration = 67,                      // declaration
        S_variable = 68,                         // variable
        S_var_enum = 69,                         // var_enum
        S_var_all = 70,                          // var_all
        S_logic = 71,                            // logic
        S_logic_all = 72,                        // logic_all
        S_logic_par = 73,                        // logic_par
        S_logic_predicates = 74,                 // logic_predicates
        S_binary_predicate = 75,                 // binary_predicate
        S_logic_unary = 76,                      // logic_unary
        S_logic_no_binary = 77,                  // logic_no_binary
        S_quantifier = 78,                       // quantifier
        S_quant_var = 79,                        // quant_var
        S_quant_var_enum = 80,                   // quant_var_enum
        S_logic_binary = 81,                     // logic_binary
        S_setexpr = 82,                          // setexpr
        S_text_function = 83,                    // text_function
        S_setexpr_enum = 84,                     // setexpr_enum
        S_setexpr_enum_min2 = 85,                // setexpr_enum_min2
        S_literal = 86,                          // literal
        S_identifier = 87,                       // identifier
        S_setexpr_binary = 88,                   // setexpr_binary
        S_setexpr_generators = 89,               // setexpr_generators
        S_enumeration = 90,                      // enumeration
        S_tuple = 91,                            // tuple
        S_boolean = 92,                          // boolean
        S_filter_expression = 93,                // filter_expression
        S_declarative = 94,                      // declarative
        S_recursion = 95,                        // recursion
        S_imperative = 96,                       // imperative
        S_imp_blocks = 97,                       // imp_blocks
        S_imp_block = 98,                        // imp_block
        S_RPE = 99,                              // RPE
        S_RCE = 100                              // RCE
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol ()
        : value ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value (std::move (that.value))
      {}
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);
      /// Constructor for valueless symbols.
      basic_symbol (typename Base::kind_type t);

      /// Constructor for symbols with semantic value.
      basic_symbol (typename Base::kind_type t,
                    YY_RVREF (semantic_type) v);

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }

      /// Destroy contents, and record that is empty.
      void clear ()
      {
        Base::clear ();
      }

      /// The user-facing name of this symbol.
      std::string name () const YY_NOEXCEPT
      {
        return RSParserImpl::symbol_name (this->kind ());
      }

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      semantic_type value;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// Default constructor.
      by_kind ();

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that);
#endif

      /// Copy constructor.
      by_kind (const by_kind& that);

      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t);

      /// Record that this symbol is empty.
      void clear ();

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {};

    /// Build a parser object.
    RSParserImpl (ParserState* state_yyarg);
    virtual ~RSParserImpl ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    RSParserImpl (const RSParserImpl&) = delete;
    /// Non copyable.
    RSParserImpl& operator= (const RSParserImpl&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param msg    a description of the syntax error.
    virtual void error (const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static std::string symbol_name (symbol_kind_type yysymbol);



    class context
    {
    public:
      context (const RSParserImpl& yyparser, const symbol_type& yyla);
      const symbol_type& lookahead () const { return yyla_; }
      symbol_kind_type token () const { return yyla_.kind (); }
      /// Put in YYARG at most YYARGN of the expected tokens, and return the
      /// number of tokens stored in YYARG.  If YYARG is null, return the
      /// number of expected tokens (guaranteed to be less than YYNTOKENS).
      int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;

    private:
      const RSParserImpl& yyparser_;
      const symbol_type& yyla_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    RSParserImpl (const RSParserImpl&);
    /// Non copyable.
    RSParserImpl& operator= (const RSParserImpl&);
#endif


    /// Stored state numbers (used for stacks).
    typedef unsigned char state_type;

    /// The arguments of the error message.
    int yy_syntax_error_arguments_ (const context& yyctx,
                                    symbol_kind_type yyarg[], int yyargn) const;

    /// Generate an error message.
    /// \param yyctx     the context in which the error occurred.
    virtual std::string yysyntax_error_ (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue);

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue);

    static const signed char yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_type enum.
    static symbol_kind_type yytranslate_ (int t);

    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *yystr);

    /// For a symbol, its name in clear.
    static const char* const yytname_[];


    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const signed char yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const short yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const short yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The (internal number of the) accessing
    // symbol of state STATE-NUM.
    static const signed char yystos_[];

    // YYR1[YYN] -- Symbol number of symbol that rule YYN derives.
    static const signed char yyr1_[];

    // YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.
    static const signed char yyr2_[];


#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200)
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range)
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1);

    /// Constants.
    enum
    {
      yylast_ = 640,     ///< Last index in yytable_.
      yynnts_ = 40,  ///< Number of nonterminal symbols.
      yyfinal_ = 87 ///< Termination state number.
    };


    // User arguments.
    ParserState* state;

  };


#line 15 "RSParserImpl.y"
} } } // ccl::rslang::detail
#line 998 "../header/RSParserImpl.h"




#endif // !YY_YY_HEADER_RSPARSERIMPL_H_INCLUDED
