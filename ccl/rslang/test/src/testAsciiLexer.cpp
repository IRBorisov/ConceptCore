#include "LexerTester.hpp"

#include "ccl/rslang/AsciiLexer.h"

class UTAsciiLexer : public LexerTester<ccl::rslang::detail::AsciiLexer> {
protected:
};

TEST_F(UTAsciiLexer, Construction) {
  static const std::string input{ "X1" };
  EXPECT_EQ(TokenID::ID_GLOBAL, ccl::rslang::detail::AsciiLexer{ input }.Stream()().id);
}

TEST_F(UTAsciiLexer, Whitespace) {
  TestToken(" ", TokenID::END);
  TestToken("\t", TokenID::END);
  TestToken("\n", TokenID::END);
  TestToken("\n \t", TokenID::END);
  TestPosition("  B  ", TokenID::BOOLEAN, StrRange{ 2, 3 });
  TestPosition("\nB\n", TokenID::BOOLEAN, StrRange{ 1, 2 });
}

TEST_F(UTAsciiLexer, Symbolic) {
  TestSingle("*", TokenID::DECART);
  TestSingle("B", TokenID::BOOLEAN);

  TestSingle("(", TokenID::PUNC_PL);
  TestSingle(")", TokenID::PUNC_PR);
  TestSingle("{", TokenID::PUNC_CL);
  TestSingle("}", TokenID::PUNC_CR);
  TestSingle("[", TokenID::PUNC_SL);
  TestSingle("]", TokenID::PUNC_SR);
  TestSingle("|", TokenID::PUNC_BAR);
  TestSingle(",", TokenID::PUNC_COMMA);
  TestSingle(";", TokenID::PUNC_SEMICOLON);

  TestSingle("D", TokenID::DECLARATIVE);
  TestSingle("R", TokenID::RECURSIVE);
  TestSingle("I", TokenID::IMPERATIVE);

  TestSingle("Z", TokenID::LIT_INTSET);
}

TEST_F(UTAsciiLexer, MultipleBoolean) {
  TestPosition("BBB", TokenID::BOOLEAN, StrRange{ 0, 1 });
  ExpectNextIDPos(TokenID::BOOLEAN, StrRange{ 1, 2 });
  ExpectNextIDPos(TokenID::BOOLEAN, StrRange{ 2, 3 });
  ExpectNextIDPos(TokenID::END, StrRange{ 3, 3 });
}

TEST_F(UTAsciiLexer, Keywords) {
  TestSingle(R"(\multiply)", TokenID::MULTIPLY);
  TestToken(R"(\multiply 1)", TokenID::MULTIPLY);

  TestSingle(R"(\A)", TokenID::FORALL);
  TestSingle(R"(\E)", TokenID::EXISTS);
  TestSingle(R"(\neg)", TokenID::NOT);

  TestSingle(R"(\plus)", TokenID::PLUS);
  TestSingle(R"(\minus)", TokenID::MINUS);
  TestSingle(R"(\gr)", TokenID::GREATER);
  TestSingle(R"(\ls)", TokenID::LESSER);
  TestSingle(R"(\ge)", TokenID::GREATER_OR_EQ);
  TestSingle(R"(\le)", TokenID::LESSER_OR_EQ);

  TestSingle(R"(\eq)", TokenID::EQUAL);
  TestSingle(R"(\noteq)", TokenID::NOTEQUAL);

  TestSingle(R"(\equiv)", TokenID::EQUIVALENT);
  TestSingle(R"(\impl)", TokenID::IMPLICATION);
  TestSingle(R"(\and)", TokenID::AND);
  TestSingle(R"(\or)", TokenID::OR);

  TestSingle(R"(\from)", TokenID::PUNC_ITERATE);
  TestSingle(R"(\in)", TokenID::IN);
  TestSingle(R"(\notin)", TokenID::NOTIN);
  TestSingle(R"(\subseteq)", TokenID::SUBSET_OR_EQ);
  TestSingle(R"(\subset)", TokenID::SUBSET);
  TestSingle(R"(\notsubset)", TokenID::NOTSUBSET);

  TestSingle(R"(\union)", TokenID::UNION);
  TestSingle(R"(\intersect)", TokenID::INTERSECTION);
  TestSingle(R"(\setminus)", TokenID::SET_MINUS);
  TestSingle(R"(\symmdiff)", TokenID::SYMMINUS);

  TestSingle(R"(\defexpr)", TokenID::PUNC_DEFINE);
  TestSingle(R"(\deftype)", TokenID::PUNC_STRUCT);

  TestSingle("card", TokenID::CARD);
  TestSingle("bool", TokenID::BOOL);
  TestSingle("red", TokenID::REDUCE);
  TestSingle("debool", TokenID::DEBOOL);

  TestSingle("{}", TokenID::LIT_EMPTYSET);
}

TEST_F(UTAsciiLexer, Identifiers) {
  TestPosition("X1", TokenID::ID_GLOBAL, StrRange{ 0, 2 });
  TestPosition("abc1", TokenID::ID_LOCAL, StrRange{ 0, 4 });
  TestPosition("_abc1_", TokenID::ID_LOCAL, StrRange{ 0, 6 });
  TestPosition("misterBob", TokenID::ID_LOCAL, StrRange{ 0, 9 });
  TestPosition("mister_Bob", TokenID::ID_LOCAL, StrRange{ 0, 10 });

  TestText("C1", TokenID::ID_GLOBAL);
  TestText("S1", TokenID::ID_GLOBAL);
  TestText("A1", TokenID::ID_GLOBAL);
  TestText("D1", TokenID::ID_GLOBAL);
  TestText("T1", TokenID::ID_GLOBAL);
  TestText("F1", TokenID::ID_FUNCTION);
  TestText("P1", TokenID::ID_PREDICATE);
  TestText("R1", TokenID::ID_RADICAL);

  TestText("misterBob", TokenID::ID_LOCAL);
  TestText("Alice", TokenID::ID_GLOBAL);
  TestText("D152", TokenID::ID_GLOBAL);

  TestToken("1", TokenID::LIT_INTEGER);
}

TEST_F(UTAsciiLexer, StructureManipulation) {
  TestPosition("Pr1", TokenID::BIGPR, StrRange{ 0, 3 });
  TestTuple("Pr1", TokenID::BIGPR, { 1 });
  TestTuple("pr1,20,3", TokenID::SMALLPR, { 1, 20, 3 });
  TestTuple("Fi1,3", TokenID::FILTER, { 1, 3 });

  TestToken("Pr", TokenID::ID_GLOBAL);
  TestToken("pr", TokenID::ID_LOCAL);
  TestToken("prag", TokenID::ID_LOCAL);
  TestToken("po1", TokenID::ID_LOCAL);
}

TEST_F(UTAsciiLexer, ReadMultipleTokens) {
  const std::string input{ "X1 1 Pr1(a)" };
  lex.SetInput(input);
  ExpectNextIDPos(TokenID::ID_GLOBAL, StrRange{ 0, 2 });
  ExpectNextIDPos(TokenID::LIT_INTEGER, StrRange{ 3, 4 });
  ExpectNextIDPos(TokenID::BIGPR, StrRange{ 5, 8 });
  ExpectNextIDPos(TokenID::PUNC_PL, StrRange{ 8, 9 });
  ExpectNextIDPos(TokenID::ID_LOCAL, StrRange{ 9, 10 });
  ExpectNextIDPos(TokenID::PUNC_PR, StrRange{ 10, 11 });
  ExpectNextID(TokenID::END);

  const std::string input2{ R"(1 \eq 1)" };
  lex.SetInput(input2);
  ExpectNextIDPos(TokenID::LIT_INTEGER, StrRange{ 0, 1 });
  ExpectNextIDPos(TokenID::EQUAL, StrRange{ 2, 5 });
  ExpectNextIDPos(TokenID::LIT_INTEGER, StrRange{ 6, 7 });
  ExpectNextID(TokenID::END);
}