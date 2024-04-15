#pragma once

#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/rslang/ErrorLogger.h"
#include "ccl/rslang/RSToken.h"

using ccl::StrRange;

template <typename Lexer>
class LexerTester : public ::testing::Test {
protected:
  using TokenID = ccl::rslang::TokenID;
  using Token = ccl::rslang::Token;
  using StrPos = ccl::StrPos;

  ccl::rslang::ErrorLogger log{};
  Lexer lex{ log.SendReporter() };
  Token curToken{};
  std::string lexInput{};

  void ExpectNextID(TokenID tid) {
    lex.lex();
    curToken = lex.MakeToken();
    EXPECT_EQ(curToken.id, tid)
      << lexInput << " " << Token::Str(tid) << R"( != )" << Token::Str(curToken.id);
  }
  void ExpectNextIDPos(TokenID tid, StrRange pos) {
    ExpectNextID(tid);
    EXPECT_EQ(curToken.pos, pos);
  }

  void TestSingle(const std::string& input, TokenID tid, StrPos len = -1) {
    if (len == -1) {
      len = static_cast<StrPos>(ssize(input));
    }
    TestPosition(input, tid, StrRange{ 0, len });
  }

  void TestToken(const std::string& input, TokenID tid) {
    lexInput = input;
    lex.SetInput(lexInput);
    ExpectNextID(tid);
  }
  void TestPosition(const std::string& input, TokenID tid, StrRange pos) {
    TestToken(input, tid);
    EXPECT_EQ(curToken.pos, pos);
  }

  void TestText(const std::string& input, TokenID tid) {
    TestText(input, tid, input);
  }
  void TestText(const std::string& input, TokenID tid, const std::string& expected) {
    TestToken(input, tid);
    EXPECT_EQ(curToken.ToString(), expected) << input;
  }
  void TestInt(const std::string& input, TokenID tid, int32_t value) {
    TestToken(input, tid);
    ASSERT_TRUE(curToken.data.IsInt()) << input;
    EXPECT_EQ(curToken.data.ToInt(), value) << input;
  }
  void TestTuple(const std::string& input, TokenID tid, const std::vector<ccl::rslang::Index>& value) {
    TestToken(input, tid);
    ASSERT_TRUE(curToken.data.IsTuple()) << input;
    EXPECT_EQ(curToken.data.ToTuple(), value) << input;
  }
};