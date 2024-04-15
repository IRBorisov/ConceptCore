#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeRSEnvironment.hpp"

class UTLiterals: public ::testing::Test {
protected:
  using Typification = ccl::rslang::Typification;
  using Syntax = ccl::rslang::Syntax;
};

TEST_F(UTLiterals, TypificationLiteral) {
  EXPECT_EQ(Typification("X1"), "X1"_t);
  EXPECT_EQ(Typification("C1"), "C1"_t);
  EXPECT_EQ(Typification::Integer(), "Z"_t);
  EXPECT_EQ(Typification("X1").Bool(), "B(X1)"_t);
  EXPECT_EQ(Typification("X1").Bool().Bool(), "BB(X1)"_t);
  EXPECT_EQ(Typification::Integer().Bool(), "B(Z)"_t);
  EXPECT_EQ(Typification::Tuple({ Typification("X1"), Typification("X2") }), "X1*X2"_t);
}

TEST_F(UTLiterals, RSlangLiteral) {
  using ccl::operator""_c17;
  EXPECT_EQ(R"(S1 \in B(X1))"_rs, u8"S1\u2208\u212C(X1)"_c17);
  EXPECT_EQ("B(Y*(X1*X2)*BB(X3))"_rs, u8"\u212C(Y\u00D7(X1\u00D7X2)\u00D7\u212C\u212C(X3))"_c17);
}