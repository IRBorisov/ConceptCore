#include "LexerTester.hpp"

#include "ccl/rslang/MathLexer.h"

#include <climits>

using ccl::operator""_c17;

class UTMathLexer : public LexerTester<ccl::rslang::detail::MathLexer> {
protected:
  using LexerEID = ccl::rslang::LexerEID;
  using MathLexer = ccl::rslang::detail::MathLexer;

protected:
  void ExpectError(const std::string& input, const LexerEID id, ccl::StrPos pos);
};

void UTMathLexer::ExpectError(const std::string& input, const LexerEID id, const ccl::StrPos pos) {
  log.Clear();
  curToken = lex(input).Stream()();
  ASSERT_FALSE(empty(log.All()));
  EXPECT_EQ(static_cast<uint32_t>(id), std::begin(log.All())->eid) << input;
  EXPECT_EQ(pos, log.FirstErrorPos()) << input;
}

TEST_F(UTMathLexer, DefaultConstruct) {
  MathLexer newLex{};
  EXPECT_EQ(newLex.LastID(), TokenID::INTERRUPT);
}

TEST_F(UTMathLexer, BasicInterface) {
  EXPECT_EQ(lex({}).lex(), TokenID::END);
  EXPECT_EQ(lex("X1").lex(), TokenID::ID_GLOBAL);
  EXPECT_EQ(lex.LastID(), TokenID::ID_GLOBAL);
  EXPECT_EQ(lex.Text(), "X1");
  EXPECT_EQ(lex.Range(), StrRange(0, 2));
  EXPECT_EQ(lex.ParseData().ToText(), "X1");
}

TEST_F(UTMathLexer, Positions) {
  EXPECT_EQ(lex(u8"\u212CX1"_c17).lex(), TokenID::BOOLEAN);
  EXPECT_EQ(lex.Range(), StrRange(0, 1));
  EXPECT_EQ(lex.RangeInBytes(), StrRange(0, 3));
  EXPECT_EQ(lex.lex(), TokenID::ID_GLOBAL);
  EXPECT_EQ(lex.Range(), StrRange(1, 3));
  EXPECT_EQ(lex.RangeInBytes(), StrRange(3, 5));
}

TEST_F(UTMathLexer, SingleSymbol) {
  TestSingle("(", TokenID::PUNC_PL);
  TestSingle(")", TokenID::PUNC_PR);
  TestSingle("{", TokenID::PUNC_CL);
  TestSingle("}", TokenID::PUNC_CR);
  TestSingle("[", TokenID::PUNC_SL);
  TestSingle("]", TokenID::PUNC_SR);
  TestSingle("|", TokenID::PUNC_BAR);
  TestSingle(",", TokenID::PUNC_COMMA);
  TestSingle(";", TokenID::PUNC_SEMICOLON);

  TestSingle("=", TokenID::EQUAL);
  TestSingle(u8"\u2260"_c17, TokenID::NOTEQUAL, 1);
  TestSingle(u8"\u2265"_c17, TokenID::GREATER_OR_EQ, 1);
  TestSingle(u8"\u2264"_c17, TokenID::LESSER_OR_EQ, 1);
  TestSingle("Z", TokenID::LIT_INTSET, 1);

  TestSingle("+", TokenID::PLUS);
  TestSingle("-", TokenID::MINUS);
  TestSingle("*", TokenID::MULTIPLY);
  TestSingle(">", TokenID::GREATER);
  TestSingle("<", TokenID::LESSER);

  TestSingle(u8"\u2200"_c17, TokenID::FORALL, 1);
  TestSingle(u8"\u2203"_c17, TokenID::EXISTS, 1);
  TestSingle("&", TokenID::AND);
  TestSingle(u8"\u2228"_c17, TokenID::OR, 1);
  TestSingle(u8"\u00AC"_c17, TokenID::NOT, 1);
  TestSingle(u8"\u21D2"_c17, TokenID::IMPLICATION, 1);
  TestSingle(u8"\u21D4"_c17, TokenID::EQUIVALENT, 1);

  TestSingle(u8"\u2205"_c17, TokenID::LIT_EMPTYSET, 1);
  TestSingle(u8"\u222A"_c17, TokenID::UNION, 1);
  TestSingle(u8"\u2229"_c17, TokenID::INTERSECTION, 1);
  TestSingle(u8"\u2206"_c17, TokenID::SYMMINUS, 1);
  TestSingle(R"(\)", TokenID::SET_MINUS);
  TestSingle(u8"\u2208"_c17, TokenID::IN, 1);
  TestSingle(u8"\u2209"_c17, TokenID::NOTIN, 1);
  TestSingle(u8"\u2282"_c17, TokenID::SUBSET, 1);
  TestSingle(u8"\u2286"_c17, TokenID::SUBSET_OR_EQ, 1);
  TestSingle(u8"\u2284"_c17, TokenID::NOTSUBSET, 1);

  TestSingle(u8"\u00D7"_c17, TokenID::DECART, 1);
  TestSingle("\xE2\x84\xAC", TokenID::BOOLEAN, 1);

  TestToken(u8":\u2208"_c17, TokenID::PUNC_ITERATE);
  TestSingle(":=", TokenID::PUNC_ASSIGN);
  TestSingle(":==", TokenID::PUNC_DEFINE);
  TestSingle("::=", TokenID::PUNC_STRUCT);
}

TEST_F(UTMathLexer, SpecialSymbols) {
  TestToken("", TokenID::END);
  TestToken("'", TokenID::INTERRUPT);
  TestToken("?", TokenID::INTERRUPT);
  TestToken("@", TokenID::INTERRUPT);
  TestToken(u8":\u2282"_c17, TokenID::INTERRUPT);
}

TEST_F(UTMathLexer, GlobalID) {
  TestText("X1", TokenID::ID_GLOBAL);
  EXPECT_TRUE(curToken.data.HasValue());
  TestPosition("X1", TokenID::ID_GLOBAL, StrRange{0, 2});

  TestText("C1", TokenID::ID_GLOBAL);
  TestText("S1", TokenID::ID_GLOBAL);
  TestText("A1", TokenID::ID_GLOBAL);
  TestText("D1", TokenID::ID_GLOBAL);
  TestText("T1", TokenID::ID_GLOBAL);
  TestText("F1", TokenID::ID_FUNCTION);
  TestText("P1", TokenID::ID_PREDICATE);
  TestText("R1", TokenID::ID_RADICAL);
}

TEST_F(UTMathLexer, LocalID) {
  TestText("a", TokenID::ID_LOCAL);
  EXPECT_TRUE(curToken.data.HasValue());

  TestText("c", TokenID::ID_LOCAL);
  TestText(u8"\u03B1"_c17, TokenID::ID_LOCAL); // greek
  TestText("word", TokenID::ID_LOCAL);
  TestText("cardinal", TokenID::ID_LOCAL);
  TestText("indexed1", TokenID::ID_LOCAL);
  TestText("two words", TokenID::ID_LOCAL, "two");
  TestText("mixed41gh12", TokenID::ID_LOCAL);
  TestText(u8"mix\u03B1d41gh12"_c17, TokenID::ID_LOCAL);
  TestText("variable_name", TokenID::ID_LOCAL);
  TestText("__variable__", TokenID::ID_LOCAL);
}

TEST_F(UTMathLexer, TextOperator) {
  TestSingle("red", TokenID::REDUCE);
  TestSingle("R", TokenID::RECURSIVE);
  TestSingle("I", TokenID::IMPERATIVE);
  TestSingle("D", TokenID::DECLARATIVE);
  TestToken("red(D1)", TokenID::REDUCE);
  TestSingle("bool", TokenID::BOOL);
  TestSingle("debool", TokenID::DEBOOL);
  TestSingle("card", TokenID::CARD);

  TestSingle("boola", TokenID::ID_LOCAL);
  TestSingle("brol", TokenID::ID_LOCAL);
}

TEST_F(UTMathLexer, StructureManipulation) {
  TestTuple("Pr1", TokenID::BIGPR, { 1 });
  TestTuple("pr1,20,3", TokenID::SMALLPR, { 1, 20, 3 });
  TestTuple("Fi1,3", TokenID::FILTER, { 1, 3 });

  TestSingle("Pr", TokenID::ID_GLOBAL);
  TestSingle("pr", TokenID::ID_LOCAL);
  TestSingle("prag", TokenID::ID_LOCAL);
  TestSingle("po1", TokenID::ID_LOCAL);
}

TEST_F(UTMathLexer, Integer) {
  EXPECT_EQ(lex("123").lex(), TokenID::LIT_INTEGER);
  EXPECT_EQ(lex.ParseData().ToInt(), 123);
  lex("42").lex();
  EXPECT_EQ(lex.ParseData().ToInt(), 42);
}

TEST_F(UTMathLexer, ReadMultipleTokens) {
  const std::string input{ "X1 1 Pr1(a)" };
  lex.SetInput(input);
  ExpectNextIDPos(TokenID::ID_GLOBAL, StrRange{ 0, 2 });
  ExpectNextIDPos(TokenID::LIT_INTEGER, StrRange{ 3, 4 });
  ExpectNextIDPos(TokenID::BIGPR, StrRange{ 5, 8 });
  ExpectNextIDPos(TokenID::PUNC_PL, StrRange{ 8, 9 });
  ExpectNextIDPos(TokenID::ID_LOCAL, StrRange{ 9, 10 });
  ExpectNextIDPos(TokenID::PUNC_PR, StrRange{ 10, 11 });
  ExpectNextID(TokenID::END);

  const std::string input2{ "1=1" };
  lex.SetInput(input2);
  ExpectNextIDPos(TokenID::LIT_INTEGER, StrRange{ 0, 1 });
  ExpectNextIDPos(TokenID::EQUAL, StrRange{ 1, 2 });
  ExpectNextIDPos(TokenID::LIT_INTEGER, StrRange{ 2, 3 });
  ExpectNextID(TokenID::END);
}

TEST_F(UTMathLexer, ReadMultipleLastSpace) {
  const std::string input = "X1 ";
  lex(input).lex();
  ExpectNextID(TokenID::END);
}

TEST_F(UTMathLexer, SkipWhiteSpaces) {
  TestPosition(" X1 ", TokenID::ID_GLOBAL, StrRange{ 1, 3 });
  TestPosition("  X1 ", TokenID::ID_GLOBAL, StrRange{ 2, 4 });
  TestPosition("\nX1\n\t", TokenID::ID_GLOBAL, StrRange{ 1, 3 });
  TestPosition("\tX1", TokenID::ID_GLOBAL, StrRange{ 1, 3 });
}

TEST_F(UTMathLexer, Errors) {
  ExpectError("@", LexerEID::unknownSymbol, 0);
  ExpectError("?", LexerEID::unknownSymbol, 0);
  ExpectError(":", LexerEID::unknownSymbol, 0);
  ExpectError("!=", LexerEID::unknownSymbol, 0);
}