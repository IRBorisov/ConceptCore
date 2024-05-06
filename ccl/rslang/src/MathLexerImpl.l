
%option fast

%option outfile=MathLexerImpl.hpp

%option namespace=ccl::rslang::detail::rslex
%option lexer=MathLexerImpl
%option token-type=ccl::rslang::TokenID

%option noindent
%option tabs=1
%option unicode
%option noline
%option nodefault
%option noyywrap

%top{

#pragma once
#include "ccl/rslang/RSToken.h"

%}

%class{
public:
  StrPos lineBase{ 0 };

  [[nodiscard]] StrRange Range() const {
    return StrRange{ 
        static_cast<StrPos>(lineBase + columno()),
        static_cast<StrPos>(lineBase + columno() + columns())
    };
  }

%}

digit       [0-9]
upper       [A-Z]
lower       [a-z\x{03B1}-\x{03C9}]
alpha       ({upper}|{lower})
alnum       (_|{digit}|{alpha})
number      {digit}+
index       {number}(,{number})*

global_id   [||{upper}--[B]]{alnum}*
local_id    (_|{lower}){alnum}*

%%

"+"                 { return TokenID::PLUS; }
"-"                 { return TokenID::MINUS; }
"*"                 { return TokenID::MULTIPLY; }
">"                 { return TokenID::GREATER; }
"<"                 { return TokenID::LESSER; }
\x{2265}            { return TokenID::GREATER_OR_EQ; }
\x{2264}            { return TokenID::LESSER_OR_EQ; }

"="                 { return TokenID::EQUAL; }
\x{2260}            { return TokenID::NOTEQUAL; }

\x{2200}            { return TokenID::FORALL; }
\x{2203}            { return TokenID::EXISTS; }

\x{00AC}            { return TokenID::NOT; }
"&"                 { return TokenID::AND; }
\x{2228}            { return TokenID::OR; }
\x{21D2}            { return TokenID::IMPLICATION; }
\x{21D4}            { return TokenID::EQUIVALENT; }

:\x{2208}           { return TokenID::PUNC_ITERATE; }
\x{2208}            { return TokenID::IN; }
\x{2209}            { return TokenID::NOTIN; }
\x{2286}            { return TokenID::SUBSET_OR_EQ; }
\x{2282}            { return TokenID::SUBSET; }
\x{2284}            { return TokenID::NOTSUBSET; }

\x{00D7}            { return TokenID::DECART; }
\x{222A}            { return TokenID::UNION; }
\x{2229}            { return TokenID::INTERSECTION; }
\x{005C}            { return TokenID::SET_MINUS; }
\x{2206}            { return TokenID::SYMMINUS; }
\x{212C}            { return TokenID::BOOLEAN; }

pr{index}           { return TokenID::SMALLPR; }
Pr{index}           { return TokenID::BIGPR; }
Fi{index}           { return TokenID::FILTER; }
card                { return TokenID::CARD; }
bool                { return TokenID::BOOL; }
red                 { return TokenID::REDUCE; }
debool              { return TokenID::DEBOOL; }

D                   { return TokenID::DECLARATIVE; }
R                   { return TokenID::RECURSIVE; }
I                   { return TokenID::IMPERATIVE; }

Z                   { return TokenID::LIT_INTSET; }
\x{2205}            { return TokenID::LIT_EMPTYSET; }
{number}            { return TokenID::LIT_INTEGER; }

F{number}           { return TokenID::ID_FUNCTION; }
P{number}           { return TokenID::ID_PREDICATE; }
R{number}           { return TokenID::ID_RADICAL; }

{global_id}         { return TokenID::ID_GLOBAL; }
{local_id}          { return TokenID::ID_LOCAL; }

":="                { return TokenID::PUNC_ASSIGN; }
":=="               { return TokenID::PUNC_DEFINE; }
"::="               { return TokenID::PUNC_STRUCT; }
"("                 { return TokenID::PUNC_PL; }
")"                 { return TokenID::PUNC_PR; }
"{"                 { return TokenID::PUNC_CL; }
"}"                 { return TokenID::PUNC_CR; }
"["                 { return TokenID::PUNC_SL; }
"]"                 { return TokenID::PUNC_SR; }
"|"                 { return TokenID::PUNC_BAR; }
","                 { return TokenID::PUNC_COMMA; }
";"                 { return TokenID::PUNC_SEMICOLON; }

\n                  { lineBase += static_cast<StrPos>(columno() + 1); }
[ \t]+              ;
<<EOF>>             { return TokenID::END; }
.                   { return TokenID::INTERRUPT; }

%%
