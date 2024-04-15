#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/rslang/Parser.h"
#include "ccl/rslang/RSGenerator.h"

using ccl::operator""_c17;

class UTParser : public ::testing::Test {
protected:
  using Typification = ccl::rslang::Typification;
  using Parser = ccl::rslang::Parser;
  using LogicT = ccl::rslang::LogicT;
  using Syntax = ccl::rslang::Syntax;
  using TokenID = ccl::rslang::TokenID;

protected:
  Parser parser{};
};

TEST_F(UTParser, EstimateSyntax) {
  EXPECT_EQ(Parser::EstimateSyntax(""), Syntax::ASCII);
  EXPECT_EQ(Parser::EstimateSyntax(R"(X1 \equal X1)"), Syntax::ASCII);
  EXPECT_EQ(Parser::EstimateSyntax("X1=X1"), Syntax::MATH);
  EXPECT_EQ(Parser::EstimateSyntax("a & b"), Syntax::MATH);
  EXPECT_EQ(Parser::EstimateSyntax("1+1"), Syntax::MATH);
  EXPECT_EQ(Parser::EstimateSyntax("1-1"), Syntax::MATH);
  EXPECT_EQ(Parser::EstimateSyntax("1>1"), Syntax::MATH);
  EXPECT_EQ(Parser::EstimateSyntax("1<1"), Syntax::MATH);
  EXPECT_EQ(Parser::EstimateSyntax(R"(1 \plus 1)"), Syntax::ASCII);
  EXPECT_EQ(Parser::EstimateSyntax(R"(X1 \in S1)"), Syntax::ASCII);
  EXPECT_EQ(Parser::EstimateSyntax(u8"X1\u00D7X1"_c17), Syntax::MATH);
}

TEST_F(UTParser, Lex) {
  {
    const std::string expr = "";
    auto stream = parser.Lex(expr);
    auto token = stream();
    EXPECT_EQ(token.id, TokenID::END);
    EXPECT_EQ(token.pos, ccl::StrRange(0, 0));
  }
  {
    const std::string expr = "X1=X1";
    auto stream = parser.Lex(expr, Syntax::MATH);
    auto token = stream();
    EXPECT_EQ(token.id, TokenID::ID_GLOBAL);
    EXPECT_EQ(token.pos, ccl::StrRange(0, 2));
    EXPECT_EQ(stream().id, TokenID::EQUAL);
    EXPECT_EQ(stream().id, TokenID::ID_GLOBAL);
  }
}

TEST_F(UTParser, Parse) {
  {
    const std::string expr = "";
    ASSERT_FALSE(parser.Parse(expr));
  }
  {
    const std::string expr = "X1=X1";
    ASSERT_TRUE(parser.Parse(expr, Syntax::MATH));
    EXPECT_EQ(ssize(parser.Errors().All()), 0);
  }
}