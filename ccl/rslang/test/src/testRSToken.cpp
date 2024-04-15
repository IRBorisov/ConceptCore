#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/rslang/RSToken.h"

using ccl::StrRange;
using ccl::Comparison;

class UTRSToken : public ::testing::Test {
protected:
  using TokenData = ccl::rslang::TokenData;
  using Token = ccl::rslang::Token;
  using TokenID = ccl::rslang::TokenID;
  using Syntax = ccl::rslang::Syntax;
  using Index = ccl::rslang::Index;
};

TEST_F(UTRSToken, TokenDataEmpty) {
  const TokenData empty{};
  EXPECT_FALSE(empty.HasValue());
  EXPECT_FALSE(empty.IsInt());
  EXPECT_FALSE(empty.IsText());
  EXPECT_EQ(empty, TokenData{});
}

TEST_F(UTRSToken, TokenDataInt) {
  const TokenData dataInt{ 42 };
  EXPECT_TRUE(dataInt.HasValue());
  EXPECT_TRUE(dataInt.IsInt());
  EXPECT_FALSE(dataInt.IsText());
  EXPECT_FALSE(dataInt.IsTuple());
  EXPECT_EQ(dataInt.ToInt(), 42);
  EXPECT_EQ(dataInt, TokenData{ 42 });
  EXPECT_NE(dataInt, TokenData{ 43 });
}

TEST_F(UTRSToken, TokenDataText) {
  const TokenData dataText{ "42" };
  EXPECT_TRUE(dataText.HasValue());
  EXPECT_FALSE(dataText.IsInt());
  EXPECT_FALSE(dataText.IsTuple());
  EXPECT_TRUE(dataText.IsText());
  EXPECT_EQ(dataText.ToText(), "42");
  EXPECT_EQ(dataText, TokenData{ "42" });
  EXPECT_NE(dataText, TokenData{ "43" });
}

TEST_F(UTRSToken, TokenDataTuple) {
  const auto vec1 = std::vector<Index>{ 1, 2, 3 };
  const auto vec2 = std::vector<Index>{ 1, 3, 2 };
  const TokenData dataTuple{ vec1 };
  EXPECT_TRUE(dataTuple.HasValue());
  EXPECT_FALSE(dataTuple.IsInt());
  EXPECT_FALSE(dataTuple.IsText());
  EXPECT_TRUE(dataTuple.IsTuple());
  EXPECT_EQ(dataTuple.ToTuple(), vec1);
  EXPECT_EQ(dataTuple, TokenData(vec1));
  EXPECT_NE(dataTuple, TokenData(vec2));
}

TEST_F(UTRSToken, FromIndexSequence) {
  const auto vec = std::vector<Index>{ 1, 20, 3, 1 };
  EXPECT_EQ(TokenData::FromIndexSequence("1,20,3,1"), TokenData(vec));
}

TEST_F(UTRSToken, TokenDataIncomparable) {
  const TokenData dataText{ "42" };
  const TokenData dataInt{ 42 };
  const TokenData dataTuple{ std::vector<Index>{ 1, 2, 3 } };
  const TokenData empty{};

  EXPECT_NE(dataInt, dataText);
  EXPECT_NE(dataInt, empty);
  EXPECT_NE(dataText, empty);
  EXPECT_NE(dataTuple, empty);
  EXPECT_NE(dataTuple, dataInt);
  EXPECT_NE(dataTuple, dataText);
}

TEST_F(UTRSToken, TokenDefault) {
  const Token empty{};
  EXPECT_EQ(empty.id, TokenID::INTERRUPT);
  EXPECT_EQ(empty.pos, ccl::StrRange{});
  EXPECT_EQ(empty.data, TokenData{});
}

TEST_F(UTRSToken, TokenCompare) {
  Token t1{}, t2{};
  EXPECT_EQ(t1, t2);
  
  t2.id = TokenID::PUNC_DEFINE;
  EXPECT_NE(t1, t2);
  t1.id = t2.id;
  EXPECT_EQ(t1, t2);

  t2.pos = ccl::StrRange{ 1,1 };
  EXPECT_NE(t1, t2);
  t1.pos = t2.pos;
  EXPECT_EQ(t1, t2);

  t2.data = TokenData{ 1 };
  EXPECT_NE(t1, t2);
  t1.data = t2.data;
  EXPECT_EQ(t1, t2);
}

TEST_F(UTRSToken, TokenInterrupt) {
  EXPECT_EQ(Token::Str(TokenID::INTERRUPT), "INTERRUPT");
}

TEST_F(UTRSToken, TokenToString) {
  const Token local{ TokenID::ID_LOCAL, StrRange{}, TokenData{"identifier"} };
  EXPECT_EQ(local.ToString(), "identifier");

  const Token global{ TokenID::ID_GLOBAL, StrRange{}, TokenData{"D1"} };
  EXPECT_EQ(global.ToString(), "D1");

  const Token integer{ TokenID::LIT_INTEGER, StrRange{}, TokenData{ 42 } };
  EXPECT_EQ(integer.ToString(), "42");

  const Token projection{ TokenID::BIGPR, StrRange{}, TokenData(std::vector<Index>{ 1 }) };
  EXPECT_EQ(projection.ToString(), "Pr1");

  const Token symbol{ TokenID::PUNC_PL, StrRange{} };
  EXPECT_EQ(symbol.ToString(), Token::Str(TokenID::PUNC_PL));
}

TEST_F(UTRSToken, TokenToStringUnicode) {
  using ccl::operator""_c17;

  EXPECT_EQ("p1", Token(TokenID::ID_LOCAL, StrRange{}, TokenData{ u8"\u03C01"_c17 }).ToString(Syntax::ASCII));
  EXPECT_EQ("aw", Token(TokenID::ID_LOCAL, StrRange{}, TokenData{ u8"\u03B1\u03C9"_c17 }).ToString(Syntax::ASCII));
  EXPECT_EQ("qu?u?p", Token(TokenID::ID_LOCAL, StrRange{}, TokenData{ u8"\u03F0"_c17 }).ToString(Syntax::ASCII));
}

TEST_F(UTRSToken, CompareOperations) {
  EXPECT_EQ(Token::CompareOperations(TokenID::INTERRUPT, TokenID::INTERRUPT), Comparison::INCOMPARABLE);
  EXPECT_EQ(Token::CompareOperations(TokenID::ID_LOCAL, TokenID::ID_LOCAL), Comparison::INCOMPARABLE);
  EXPECT_EQ(Token::CompareOperations(TokenID::ID_LOCAL, TokenID::PLUS), Comparison::INCOMPARABLE);
  EXPECT_EQ(Token::CompareOperations(TokenID::PLUS, TokenID::MINUS), Comparison::EQUAL);
  EXPECT_EQ(Token::CompareOperations(TokenID::MULTIPLY, TokenID::MINUS), Comparison::GREATER);
  EXPECT_EQ(Token::CompareOperations(TokenID::MINUS, TokenID::MULTIPLY), Comparison::LESS);
}