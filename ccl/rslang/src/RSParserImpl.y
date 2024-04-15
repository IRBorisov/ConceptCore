%require "3.2"

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

%code top {
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

}

%code requires{
#include "ccl/rslang/SyntaxTree.h"
#include "ccl/rslang/ParserState.hpp"

namespace ccl::rslang::detail {

#define YYSTYPE RawNode
struct ParserState;

RawNode AddNode(TokenID token, RawNode son);
RawNode AddNode(TokenID token, RawNode son1, RawNode son2);

RawNode BinaryOperation(RawNode op1, RawNode operation, RawNode op2);
RawNode UnaryOperation(RawNode operation, RawNode operand);
RawNode RemoveBrackets(RawNode br1, RawNode operand, RawNode br2);
RawNode ReplaceBrackets(TokenID token, RawNode br1, RawNode argList, RawNode br2);
RawNode Enumeration(TokenID token, RawNode el1, RawNode el2);
RawNode Decartian(RawNode op1, RawNode decart, RawNode op2);

RawNode Quantifier(RawNode quant, RawNode declaration, RawNode domain, RawNode predicate);
RawNode FunctionDeclaration(RawNode start, RawNode argdecl, RawNode expr);
RawNode FunctionCall(RawNode function, RawNode args, RawNode rs);
RawNode FilterCall(RawNode filter, RawNode params, RawNode argument, RawNode rp);
RawNode TextOperator(RawNode oper, RawNode args, RawNode rp);
RawNode TermDeclaration(RawNode lc, RawNode declaration, RawNode domain, RawNode predicate, RawNode rc);
RawNode FullRecursion(RawNode rec, RawNode localid, RawNode domain, RawNode condition, RawNode iteration, RawNode rc);
RawNode ShortRecursion(RawNode rec, RawNode localid, RawNode domain, RawNode iteration, RawNode rc);
RawNode Imperative(RawNode imp, RawNode value, RawNode actions, RawNode rc);

int yylex(RawNode* yylval, ParserState* state);

} // namespace ccl::rslang::detail

}

//-------------- Идентификаторы -----------------------------------
%token    LOCAL      // Имя локальной переменной
      GLOBAL          // Имя глобальной переменной
      FUNCTION    // Терм-функция
      PREDICATE    // Пердикат-функция
      RADICAL      // Радикал для шаблонов функций и предикатов
      INTEGER
      INFINITY
      EMPTYSET

//------------------ Арифметика ------------------------------
%left    PLUS  MINUS
%left    MULTIPLY

// ----------- Целочисленные предикаты -----------------------
%token    GREATER
      LESSER
      GREATER_OR_EQ
      LESSER_OR_EQ

// --------------- Сравнение ---------------------------------
%token    EQUAL
      NOTEQUAL

//------------- Логические предикаты ------------------------
%token    FORALL      // Квантор всеобщности
      EXISTS      // Квантор существования
%right    NOT
%left    EQUIVALENT
%left    IMPLICATION
%left    OR
%left    AND

//-------------- Предикаты теории множеств -------------------
%token    IN
      NOTIN
      SUBSET
      SUBOR_EQ 
      NOTSUBSET

//-------------- Операции теории множеств -------------------
%left    DECART UNION INTERSECTION SET_MINUS SYMMINUS
%right    BOOLEAN         // Булеан

//------------------ Родоструктурные операции -------------------
%token    BIGPR      // Большая проекция
      SMALLPR      // Малая проекция
      FILTER      // Фильтр
      CARD            // Кардинальность (мощность множества)
      BOOL      // синглетон
      DEBOOL      // десинглетон
      RED        // reduce (множество-сумма)

//------------------ Родоструктурные конструкции -------------------
%token    DECLARATIVE    // выражение терма
      RECURSIVE    // рекурсивное определение
      IMPERATIVE    // императивное определение

//-------------- Пунктуация ---------------------------------
%token    DEFINE      // Определение глобального идентификатора
      STRUCT      // Определение глобальной структуры
      ASSIGN          // Знак присвоения
      ITERATE         // Знак перебора
      LP        // Открывающая круглая скобка
      RP        // Закрывающая круглая скобка
      LC        // Открывающая фигурная скобка
      RC        // Закрывающая фигурная скобка
      LS        // Открывающая квадратная скобка
      RS        // Закрывающая квадратная скобка
      BAR        // Вертикальная черта
%left    COMMA           // Запятая
%left    SEMICOLON       // Точка с запятой


/////////////////////////////////////////////////////////////////////////////
//---------------- Правила грамматики ---------------------------------------
%%

//--------------- Выражение ------------------------------
expression    : global_declaration
        | term_or_logic              { state->FinalizeExpression($1); }
        | function_decl              { state->FinalizeExpression($1); }
        ;

term_or_logic  : logic
        | term
        ;

function_decl  : LS arg_declaration RS term_or_logic  { $$ = FunctionDeclaration($1, $2, $4); }
        | LS error                { state->OnError(ParseEID::expectedDeclaration); YYABORT; }
        ;

arg_declaration  : declaration              { $$ = AddNode(TokenID::NT_ARGUMENTS, $1); }
        | arg_declaration COMMA declaration    { $$ = Enumeration(TokenID::NT_ARGUMENTS, $1, $3); }
        | arg_declaration COMMA error      { state->OnError(ParseEID::expectedLocal); YYABORT; }
        ;
declaration    : LOCAL IN term              { $$ = AddNode(TokenID::NT_ARG_DECL, $1, $3); }
        | LOCAL error              { state->OnError(ParseEID::expectedDeclaration); YYABORT; }
        ;

//--------------- Выражение конституэнты ------------------------------
global_declaration : GLOBAL DEFINE            { state->FinalizeCstEmpty($1, $2); }
        | GLOBAL STRUCT term          { state->FinalizeCstExpression($1, $2, $3); }
        | GLOBAL DEFINE term_or_logic      { state->FinalizeCstExpression($1, $2, $3); }
        | function_name DEFINE function_decl  { state->FinalizeCstExpression($1, $2, $3); }
        ;

//--------------- Логическое выражение -----------------------------------------
logic      : logic_predicates
        | logic_unary
        | logic_binary
        ;
logic_all    : logic
        | logic_par
        ;
logic_no_binary: logic_predicates
        | logic_unary
        | logic_par
        ;
logic_par    : LP logic_binary RPE          { $$ = RemoveBrackets($1, $2, $3); }
        | LP logic_predicates RPE        { $$ = RemoveBrackets($1, $2, $3); }
        ;
logic_unary    : NOT logic_no_binary          { $$ = UnaryOperation($1, $2);}
        | quantifier quant_var IN term logic_no_binary
                            { $$ = Quantifier($1, $2, $4, $5);}
        | quantifier error            { state->OnError(ParseEID::invalidQuantifier); YYABORT; }
        | PREDICATE LS term_enum RS        { $$ = FunctionCall($1, $3, $4); }    
        ;
logic_predicates: term binary_predicate term      { $$ = BinaryOperation($1, $2, $3); }
        ;
logic_binary  : logic_all EQUIVALENT logic_all    { $$ = BinaryOperation($1, $2, $3); }
        | logic_all IMPLICATION logic_all    { $$ = BinaryOperation($1, $2, $3); }
        | logic_all OR logic_all        { $$ = BinaryOperation($1, $2, $3); }
        | logic_all AND logic_all        { $$ = BinaryOperation($1, $2, $3); }
        ;
//--------------- Декларация локальной переменной -----------------------------
quant_var    : variable  
        | quant_var_enum
        | LP error                { state->OnError(ParseEID::invalidQuantifier); YYABORT; }
        ;
quant_var_enum  : quant_var COMMA quant_var        { $$ = Enumeration(TokenID::NT_ENUM_DECL, $1, $3); }
        | quant_var COMMA error          { state->OnError(ParseEID::expectedLocal); YYABORT; }
        ;

//-------------------- Термы РС -------------------------------------
term      : literal
        | identifier
        | binary_operation
        | typed_constructors
        | FUNCTION LS term_enum RS        { $$ = FunctionCall($1, $3, $4); }  
        | operation_name LP term RPE      { $$ = TextOperator($1, $3, $4); }
        ;
//--------------- Двухместная операция ------------------------------
binary_operation: term PLUS term             { $$ = BinaryOperation($1, $2, $3); }
        | term MINUS term             { $$ = BinaryOperation($1, $2, $3); }
        | term MULTIPLY term           { $$ = BinaryOperation($1, $2, $3); }
        | term UNION term             { $$ = BinaryOperation($1, $2, $3); }
        | term SET_MINUS term           { $$ = BinaryOperation($1, $2, $3); }
        | term SYMMINUS term          { $$ = BinaryOperation($1, $2, $3); }
        | term INTERSECTION term         { $$ = BinaryOperation($1, $2, $3); }
        | term DECART term            { $$ = Decartian($1, $2, $3);}
        | LP binary_operation RPE        { $$ = RemoveBrackets($1, $2, $3); }
        ;
//--------------- Структурные конструкции ------------------------------
typed_constructors : enumeration
        | tuple
        | boolean
        | FILTER LS term_enum RS LP term RPE  { $$ = FilterCall($1, $3, $6, $7); }
        | declarative
        | imperative
        | recursion
        ;
enumeration    : LC term_enum RC            { $$ = ReplaceBrackets(TokenID::NT_ENUMERATION, $1, $2, $3); }
        ;
tuple      : LP term_enum_min2 RPE          { $$ = ReplaceBrackets(TokenID::NT_TUPLE, $1, $2, $3); }
        ;
boolean      : BOOLEAN LP term RPE          { $$ = TextOperator($1, $3, $4); }
        | BOOLEAN boolean            { $$ = UnaryOperation($1, $2); }
        ;
term_enum    : term                  { $$ = AddNode(TokenID::INTERRUPT, $1); }
        | term_enum_min2        
        ;
term_enum_min2  : term_enum COMMA term          { $$ = Enumeration(TokenID::INTERRUPT, $1, $3); }
        ;
declarative    : LC LOCAL IN term BAR logic RCE    { $$ = TermDeclaration($1, $2, $4, $6, $7); }
        | DECLARATIVE LC variable IN term BAR logic RCE
                            { $$ = TermDeclaration($1, $3, $5, $7, $8); }
        ;
recursion    : RECURSIVE LC variable ASSIGN term BAR logic BAR term RCE
                            { $$ = FullRecursion($1, $3, $5, $7, $9, $10); }
        | RECURSIVE LC variable ASSIGN term BAR term RCE
                            { $$ = ShortRecursion($1, $3, $5, $7, $8); }
        ;
imperative    : IMPERATIVE LC term BAR imp_blocks RCE  { $$ = Imperative($1, $3, $5, $6); }
        ;
imp_blocks    : imp_block                { $$ = AddNode(TokenID::INTERRUPT, $1); }
        | imp_blocks SEMICOLON imp_blocks    { $$ = Enumeration(TokenID::INTERRUPT, $1, $3); }
        ;
imp_block    : LOCAL ITERATE term          { $$ = AddNode(TokenID::NT_IMP_DECLARE, $1, $3); }
        | LOCAL ASSIGN term            { $$ = AddNode(TokenID::NT_IMP_ASSIGN, $1, $3); }
        | logic                  { $$ = AddNode(TokenID::NT_IMP_LOGIC, $1); }
        ;
variable    : LOCAL
        | LP var_enum RPE            { $$ = ReplaceBrackets(TokenID::NT_TUPLE_DECL, $1, $2, $3); }
        ;
var_enum    : var_all COMMA var_all          { $$ = Enumeration(TokenID::INTERRUPT, $1, $3); }
        | var_all COMMA error          { state->OnError(ParseEID::expectedLocal); YYABORT; }
        ;
var_all      : variable
        | var_enum
        ;

//------------ Идентификаторы -------------------
function_name  : FUNCTION
        | PREDICATE
        ;
global_id    : GLOBAL
        | function_name
        | RADICAL
        ;
identifier    : global_id    
        | LOCAL
        ;
literal      : INTEGER
        | EMPTYSET
        | INFINITY
        ;
//------------- Кванторы --------------------------
quantifier    : FORALL
        | EXISTS
        ;
//------------ Встроенные функции ----------------
operation_name  : BOOL
        | DEBOOL
        | RED
        | BIGPR
        | SMALLPR
        | CARD
        ;
//--------------- Двухместный предикат --------
binary_predicate : IN
        | NOTIN
        | SUBSET
        | SUBOR_EQ
        | NOTSUBSET
        | NOTEQUAL
        | EQUAL
        | GREATER
        | LESSER
        | GREATER_OR_EQ
        | LESSER_OR_EQ
        ;
//--------------- Скобки ----------------
RPE        : RP
        | error                  { state->OnError(ParseEID::missingParanthesis); YYABORT; }
        ;
RCE        : RC
        | error                  { state->OnError(ParseEID::missingCurlyBrace); YYABORT; }
        ;

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