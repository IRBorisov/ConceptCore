#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/Substitutes.hpp"

TEST(UTSubstitutes, CreateTranslator) {
  const ccl::StrSubstitutes subst{ {"X1", "X2"}, {"X2","X3"} };
  const auto translator = ccl::CreateTranslator(subst);
  ASSERT_TRUE(translator("X1").has_value());
  ASSERT_FALSE(translator("X3").has_value());
  EXPECT_EQ(translator("X1").value(), "X2");
  EXPECT_EQ(translator("X2").value(), "X3");
}

TEST(UTSubstitutes, CreateMoveTranslator) {
  const auto translator = ccl::CreateTranslator({ {"X1", "X2"}, {"X2","X3"} });
  ASSERT_TRUE(translator("X1").has_value());
  ASSERT_FALSE(translator("X3").has_value());
  EXPECT_EQ(translator("X1").value(), "X2");
  EXPECT_EQ(translator("X2").value(), "X3");
}