%require "3.7"

///////////////////////////////////////////////////////////
// ------------- Generator Definitions --------------------
///////////////////////////////////////////////////////////

%output  "RSParserImpl.cpp"
%defines "../header/RSParserImpl.h"

%language "c++"
%skeleton "lalr1.cc"
%define parse.error verbose
%define parse.assert

%define api.namespace {ccl::rslang::detail}
%define api.token.prefix {RST_}
%define api.parser.class {RSParserImpl}
%param {ParserState* state}


///////////////////////////////////////////////////////////
// ------------- Code Wrapper at FileStart ----------------
///////////////////////////////////////////////////////////
%code top {
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

}


///////////////////////////////////////////////////////////
// ------------- Semantic Actions -------------------------
///////////////////////////////////////////////////////////
%code requires{
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
    RawNode args,
    RawNode rs
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
    RawNode rc
);
RawNode TupleDeclaration(
    ParserState* state,
    RawNode tuple
);

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
}


///////////////////////////////////////////////////////////
// ------------- Terminal Tokens --------------------------
///////////////////////////////////////////////////////////

// ------------- Identifiers ------------------------------
%token
    LOCAL           // Local identifier
    GLOBAL          // Global identifier
    FUNCTION        // Term-function
    PREDICATE       // Predicate-function
    RADICAL         // Templated argument identifier
    INTEGER         // Integer literal
    INTSET          // Set of all integers
    EMPTYSET        // Empty set


// ------------- Arithmetic operations --------------------
%left PLUS MINUS    // Add and Subtract
%left MULTIPLY      // Multiply


// ------------- Integer predicates -----------------------
%token
    GREATER         // IsGreater
    LESSER          // IsLesser
    GREATER_OR_EQ   // IsGreater or IsEqual
    LESSER_OR_EQ    // IsLesser or IsEqual


// ------------- Equality predicate -----------------------
%token
    EQUAL           // IsEqual
    NOTEQUAL        // NotEqual


// ------------- First order logic ------------------------
%token
    FORALL          // UniversalQuantifier
    EXISTS          // ExistentialQuantifier

%right
    NOT             // Negation

%left
    EQUIVALENT      // IsEquivalent

%left
    IMPLICATION     // Implies

%left
    OR              // LogicDisjunction

%left
    AND             // LogicConjunction


// ------------- Set theory predicates --------------------
%token
    IN              // IsElement
    NOTIN           // NotElement
    SUBSET          // IsSubset and NotEqual
    SUBSET_OR_EQ    // IsSubset
    NOTSUBSET       // NotSubset


// ------------- Set species operations -------------------
%left
    DECART          // CartesianProduct
    UNION           // SetDisjunction
    INTERSECTION    // SetConjunction
    SET_MINUS       // SetSubtraction
    SYMMINUS        // SetSymmetricDifference

%right
    BOOLEAN         // PowerSet


// ------------- Structural operations --------------------
%token
    BIGPR           // SetProjection
    SMALLPR         // CortegeProjection
    FILTER          // Filter
    CARD            // SetCardinality
    BOOL            // Singleton
    DEBOOL          // ExtractSingleElement
    RED             // SetReduction


// ------------- Definition constructions -----------------
%token
    DECLARATIVE     // DeclarativeDefinition
    RECURSIVE       // RecursiveDefinition
    IMPERATIVE      // ImperativeDefinition


// ------------- Imperative operators -------------------
    ITERATE         // SetIteration (IsElement)
    ASSIGN          // Assignment (IsEqual)


// ------------- Punctuation and Syntactic sugar ----------
%token
    DEFINE          // Global definition separator
    STRUCT          // Global structure domain separator
    LP              // (
    RP              // )
    LC              // {
    RC              // }
    LS              // [
    RS              // ]
    BAR             // |
%left
    COMMA           // ,
%left
    SEMICOLON       // ;


%%
///////////////////////////////////////////////////////////////////////////////
// ------------------------- Grammar Rules ------------------------------------
///////////////////////////////////////////////////////////////////////////////

// ------------------------- Language Expression ------------------------------
expression
    : global_declaration
    | no_declaration                        { if(!state->FinalizeExpression($1)) YYABORT; }
    ;

no_declaration
    : logic_or_setexpr
    | function_definition
    ;

global_declaration
    : global_name DEFINE                    { if(!state->FinalizeCstEmpty($1, $2)) YYABORT; }
    | global_name DEFINE no_declaration     { if(!state->FinalizeCstExpression($1, $2, $3)) YYABORT; }
    | global_name STRUCT no_declaration     { if(!state->FinalizeCstExpression($1, $2, $3)) YYABORT; }
    ;

logic_or_setexpr
    : logic
    | setexpr
    ;

function_definition
    : LS arguments RS logic_or_setexpr      { $$ = FunctionDeclaration($1, $2, $4); }
    | LS error                              { state->OnError(ParseEID::expectedDeclaration); YYABORT; }
    ;

// -------------------------  Variables and arguments -------------------------
arguments
    : declaration                           { $$ = AddNode(TokenID::NT_ARGUMENTS, $1); }
    | arguments COMMA declaration           { $$ = Enumeration(TokenID::NT_ARGUMENTS, $1, $3); }
    | arguments COMMA error                 { state->OnError(ParseEID::expectedLocal); YYABORT; }
    ;
declaration
    : LOCAL IN setexpr                      { $$ = AddNode(TokenID::NT_ARG_DECL, $1, $3); }
    | LOCAL error                           { state->OnError(ParseEID::expectedDeclaration); YYABORT; }
    ;

variable
    : LOCAL
    | tuple                                 { $$ = TupleDeclaration(state, $1); if (!$$) YYABORT; }
    ;
variable_pack
    : variable
    | variable_pack COMMA variable_pack     { $$ = Enumeration(TokenID::NT_ENUM_DECL, $1, $3); }
    | variable_pack COMMA error             { state->OnError(ParseEID::expectedLocal); YYABORT; }
    ;

// ------------------------- Logic Expressions --------------------------------
logic
    : logic_predicates
    | logic_unary
    | logic_binary
    ;
logic_no_binary
    : logic_predicates
    | logic_unary
    | logic_par
    ;
logic_all
    : logic
    | logic_par
    ;
logic_par
    : LP logic_binary RPE                   { $$ = RemoveBrackets($1, $2, $3); }
    | LP logic_predicates RPE               { $$ = RemoveBrackets($1, $2, $3); }
    ;

logic_predicates
    : setexpr binary_predicate setexpr      { $$ = BinaryOperation($1, $2, $3); }
    | variable ITERATE setexpr              { $$ = BinaryOperation($1, $2, $3); }
    | variable ASSIGN setexpr               { $$ = BinaryOperation($1, $2, $3); }
    ;
binary_predicate
    : IN
    | NOTIN
    | SUBSET
    | SUBSET_OR_EQ
    | NOTSUBSET
    | NOTEQUAL
    | EQUAL
    | GREATER
    | LESSER
    | GREATER_OR_EQ
    | LESSER_OR_EQ
    ;

logic_unary
    : NOT logic_no_binary                   { $$ = UnaryOperation($1, $2);}
    | quantifier variable_pack IN setexpr logic_no_binary { $$ = Quantifier($1, $2, $4, $5);}
    | quantifier error                      { state->OnError(ParseEID::invalidQuantifier); YYABORT; }
    | PREDICATE LS setexpr_enum RS          { $$ = FunctionCall($1, $3, $4); }    
    ;
quantifier
    : FORALL
    | EXISTS
    ;

logic_binary
    : logic_all EQUIVALENT logic_all        { $$ = BinaryOperation($1, $2, $3); }
    | logic_all IMPLICATION logic_all       { $$ = BinaryOperation($1, $2, $3); }
    | logic_all OR logic_all                { $$ = BinaryOperation($1, $2, $3); }
    | logic_all AND logic_all               { $$ = BinaryOperation($1, $2, $3); }
    ;


// ------------------------- Set Expressions ----------------------------------
setexpr
    : literal
    | identifier
    | setexpr_binary
    | setexpr_generators
    | FUNCTION LS setexpr_enum RS           { $$ = FunctionCall($1, $3, $4); }  
    | text_function LP setexpr RPE          { $$ = TextOperator($1, $3, $4); }
    ;
text_function
    : BOOL
    | DEBOOL
    | RED
    | BIGPR
    | SMALLPR
    | CARD
    ;
setexpr_enum
    : setexpr                               { $$ = AddNode(TokenID::INTERRUPT, $1); }
    | setexpr_enum_min2        
    ;
setexpr_enum_min2
    : setexpr_enum COMMA setexpr            { $$ = Enumeration(TokenID::INTERRUPT, $1, $3); }
    ;

global_name
    : GLOBAL
    | FUNCTION
    | PREDICATE
    ;

literal
    : INTEGER
    | EMPTYSET
    | INTSET
    ;

identifier
    : global_name
	| LOCAL
    | RADICAL
    ;

setexpr_binary
    : setexpr PLUS setexpr                  { $$ = BinaryOperation($1, $2, $3); }
    | setexpr MINUS setexpr                 { $$ = BinaryOperation($1, $2, $3); }
    | setexpr MULTIPLY setexpr              { $$ = BinaryOperation($1, $2, $3); }
    | setexpr UNION setexpr                 { $$ = BinaryOperation($1, $2, $3); }
    | setexpr SET_MINUS setexpr             { $$ = BinaryOperation($1, $2, $3); }
    | setexpr SYMMINUS setexpr              { $$ = BinaryOperation($1, $2, $3); }
    | setexpr INTERSECTION setexpr          { $$ = BinaryOperation($1, $2, $3); }
    | setexpr DECART setexpr                { $$ = Decartian($1, $2, $3);}
    | LP setexpr_binary RPE                 { $$ = RemoveBrackets($1, $2, $3); }
    ;

setexpr_generators
    : enumeration
    | tuple
    | boolean
    | filter_expression
    | declarative
    | imperative
    | recursion
    ;
enumeration
    : LC setexpr_enum RC                    { $$ = ReplaceBrackets(TokenID::NT_ENUMERATION, $1, $2, $3); }
    ;
tuple
    : LP setexpr_enum_min2 RPE              { $$ = ReplaceBrackets(TokenID::NT_TUPLE, $1, $2, $3); }
    ;
boolean
    : BOOLEAN LP setexpr RPE                { $$ = TextOperator($1, $3, $4); }
    | BOOLEAN boolean                       { $$ = UnaryOperation($1, $2); }
    ;
filter_expression
    : FILTER LS setexpr_enum RS LP setexpr RPE { $$ = FilterCall($1, $3, $6, $7); }
    ;

declarative
    : LC LOCAL IN setexpr BAR logic RCE     { $$ = TermDeclaration($1, $2, $4, $6, $7); }
    | DECLARATIVE LC variable IN setexpr BAR logic RCE { $$ = TermDeclaration($1, $3, $5, $7, $8); }
    ;
recursion
    : RECURSIVE LC variable ASSIGN setexpr BAR logic BAR setexpr RCE { $$ = FullRecursion($1, $3, $5, $7, $9, $10); }
    | RECURSIVE LC variable ASSIGN setexpr BAR setexpr RCE { $$ = ShortRecursion($1, $3, $5, $7, $8); }
    ;
imperative
    : IMPERATIVE LC setexpr BAR imp_blocks RCE { $$ = Imperative($1, $3, $5, $6); }
    ;
imp_blocks
    : logic                                 { $$ = AddNode(TokenID::INTERRUPT, $1); }
    | imp_blocks SEMICOLON imp_blocks       { $$ = Enumeration(TokenID::INTERRUPT, $1, $3); }
    ;


// ------------------------- Error helpers ------------------------------------
RPE
    : RP
    | error { state->OnError(ParseEID::missingParenthesis); YYABORT; }
    ;
RCE
    : RC
    | error { state->OnError(ParseEID::missingCurlyBrace); YYABORT; }
    ;


///////////////////////////////////////////////////////////
// ------------- Code Wrapper at FileEnd ----------------
///////////////////////////////////////////////////////////
%%

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

#ifdef __clang__
  #pragma clang diagnostic pop
#endif

#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic pop
#endif