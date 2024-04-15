#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/semantic/RSForm.h"
#include "RSLHelper.hpp"

class UTRSOperationsFacet: public ::testing::Test {
protected:
  using CstType = ccl::semantic::CstType;
  using EquationOptions = ccl::ops::EquationOptions;
  using RSForm = ccl::semantic::RSForm;
  using rsOperationFacet = ccl::semantic::rsOperationFacet;

protected:
  RSForm schema{};
  rsOperationFacet& ops{ schema.Ops() };
  const ccl::EntityUID x1{ schema.Emplace(CstType::base) };
  const ccl::EntityUID x2{ schema.Emplace(CstType::base) };
  const ccl::EntityUID s1{ schema.Emplace(CstType::structured, "B(X1)"_rs) };
};

TEST_F(UTRSOperationsFacet, Merge) {
  const RSForm copy = schema;
  const auto size = std::ssize(schema.Core());
  const auto translations = ops.MergeWith(copy);
  EXPECT_EQ(std::ssize(translations), size);
  EXPECT_EQ(std::ssize(schema.Core()), 2 * size);
  EXPECT_NE(translations(x1), x1);
  EXPECT_NE(translations(x2), x2);
  EXPECT_NE(translations(s1), s1);
}

TEST_F(UTRSOperationsFacet, MergeSelf) {
  const auto size = std::ssize(schema.Core());
  const auto translations = ops.MergeWith(schema);
  EXPECT_EQ(std::ssize(translations), size);
  EXPECT_EQ(std::ssize(schema.Core()), 2 * size);
}

TEST_F(UTRSOperationsFacet, IsEquatable) {
  EXPECT_TRUE(ops.IsEquatable(EquationOptions{ x1, x2 }));
  EXPECT_TRUE(ops.IsEquatable(EquationOptions{ x2, s1 }));
  EXPECT_FALSE(ops.IsEquatable(EquationOptions{ x1, s1 }));
  EXPECT_FALSE(ops.IsEquatable(EquationOptions{}));
  EXPECT_FALSE(ops.IsEquatable(EquationOptions{ x1, x1 }));
}

TEST_F(UTRSOperationsFacet, Equate) {
  EXPECT_FALSE(ops.Equate(EquationOptions{}).has_value());
  EXPECT_FALSE(ops.Equate(EquationOptions{x1, x1}).has_value());
  EXPECT_TRUE(ops.Equate(EquationOptions{x1, x2}).has_value());
  EXPECT_FALSE(schema.Contains(x1));
  EXPECT_TRUE(schema.Contains(x2));
}