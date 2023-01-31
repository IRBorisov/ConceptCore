
%option fast

%option outfile=AsciiLexerImpl.hpp

%option namespace=ccl::rslang::asciilex
%option lexer=AsciiLexerImpl
%option token-type=ccl::rslang::TokenID

%option noindent
%option noline
%option nodefault
%option noyywrap

%top{

#pragma once
#include "ccl/rslang/RSToken.h"

%}

%class{
public:
	[[nodiscard]] StrRange Range() const { 
	 // Note: returning byte position, not code point! Assume input is ASCII
		return StrRange{ static_cast<StrPos>(matcher().first()),
						static_cast<StrPos>(matcher().last()) };
	}
%}

ws				[ \t\r\n]+
digit			[0-9]
upper			[A-Z]
lower			[a-z]
alpha			({upper}|{lower})
alnum			(_|{digit}|{alpha})
number			{digit}+
index			{number}(,{number})*

global_id		[||{upper}--[B]]{alnum}*
local_id		(_|{lower}){alnum}*

%%

"\A"			{ return TokenID::FORALL; }
"\E"			{ return TokenID::EXISTS; }

"\neg"			{ return TokenID::NOT; }
"&&"			{ return TokenID::AND; }
"||"			{ return TokenID::OR; }
"=>"			{ return TokenID::IMPLICATION; }
"<=>"			{ return TokenID::EQUIVALENT; }

"+"				{ return TokenID::PLUS; }
"-"				{ return TokenID::MINUS; }
"\multiply"		{ return TokenID::MULTIPLY; }
">"				{ return TokenID::GREATER; }
"<"				{ return TokenID::LESSER; }
">="			{ return TokenID::GREATER_OR_EQ; }
"<="			{ return TokenID::LESSER_OR_EQ; }

"="				{ return TokenID::EQUAL; }
"!="			{ return TokenID::NOTEQUAL; }

"\in"			{ return TokenID::IN; }
"\notin"		{ return TokenID::NOTIN; }
"\subseteq"		{ return TokenID::SUBSET_OR_EQ; }
"\subset"		{ return TokenID::SUBSET; }
"\notsubset"	{ return TokenID::NOTSUBSET; }

"*"				{ return TokenID::DECART; }
"\union"		{ return TokenID::UNION; }
"\intersect"	{ return TokenID::INTERSECTION; }
"\setminus"		{ return TokenID::SET_MINUS; }
"\symmdiff"		{ return TokenID::SYMMINUS; }
"B"				{ return TokenID::BOOLEAN; }

pr{index}		{ return TokenID::SMALLPR; }
Pr{index}		{ return TokenID::BIGPR; }
Fi{index}		{ return TokenID::FILTER; }
card			{ return TokenID::CARD; }
bool			{ return TokenID::BOOL; }
red				{ return TokenID::REDUCE; }
debool			{ return TokenID::DEBOOL; }

D				{ return TokenID::DECLARATIVE; }
R				{ return TokenID::RECURSIVE; }
I				{ return TokenID::IMPERATIVE; }

Z				{ return TokenID::LIT_INTSET; }
"{}"			{ return TokenID::LIT_EMPTYSET; }
{number}		{ return TokenID::LIT_INTEGER; }

F{number}		{ return TokenID::ID_FUNCTION; }
P{number}		{ return TokenID::ID_PREDICATE; }
R{number}		{ return TokenID::ID_RADICAL; }

{global_id}		{ return TokenID::ID_GLOBAL; }
{local_id}		{ return TokenID::ID_LOCAL; }

":="			{ return TokenID::PUNC_ASSIGN; }
"\from"			{ return TokenID::PUNC_ITERATE; }
":=="			{ return TokenID::PUNC_DEFINE; }
"::="			{ return TokenID::PUNC_STRUCT; }
"("				{ return TokenID::PUNC_PL; }
")"				{ return TokenID::PUNC_PR; }
"{"				{ return TokenID::PUNC_CL; }
"}"				{ return TokenID::PUNC_CR; }
"["				{ return TokenID::PUNC_SL; }
"]"				{ return TokenID::PUNC_SR; }
"|"				{ return TokenID::PUNC_BAR; }
","				{ return TokenID::PUNC_COMMA; }
";"				{ return TokenID::PUNC_SEMICOLON; }

{ws}			;
<<EOF>>			{ return TokenID::END; }
.				{ return TokenID::INTERRUPT; }

%%