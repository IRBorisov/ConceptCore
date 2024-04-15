#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/ops/RSOperations.h"
#include "RSLHelper.hpp"

class UTExtractBasis : public ::testing::Test {
protected:
  using RSForm = ccl::semantic::RSForm;
  using CstType = ccl::semantic::CstType;
  using OpExtractBasis = ccl::ops::OpExtractBasis;

protected:
  RSForm schema{};

  const ccl::EntityUID x1 = schema.Emplace(CstType::base);
  const ccl::EntityUID x2 = schema.Emplace(CstType::base);
  const ccl::EntityUID s1 = schema.Emplace(CstType::structured, "B(X1)"_rs);
  const ccl::EntityUID d1 = schema.Emplace(CstType::term);
  const ccl::EntityUID d2 = schema.Emplace(CstType::term, "X1 invalid");
  const ccl::EntityUID d3 = schema.Emplace(CstType::term, R"(D2 \setminus D2)"_rs);
};

TEST_F(UTExtractBasis, IsCorrectlyDefined) {
  EXPECT_FALSE(OpExtractBasis(schema, {}).IsCorrectlyDefined());
  EXPECT_TRUE(OpExtractBasis(schema, { x1 }).IsCorrectlyDefined());
  EXPECT_TRUE(OpExtractBasis(schema, { d1 }).IsCorrectlyDefined());

  EXPECT_TRUE(OpExtractBasis(schema, { s1 }).IsCorrectlyDefined());
  EXPECT_TRUE(OpExtractBasis(schema, { d2 }).IsCorrectlyDefined());

  EXPECT_TRUE(OpExtractBasis(schema, { x1, x2 }).IsCorrectlyDefined());
  EXPECT_TRUE(OpExtractBasis(schema, { x1, d1 }).IsCorrectlyDefined());
  EXPECT_TRUE(OpExtractBasis(schema, { x1, s1 }).IsCorrectlyDefined());
  EXPECT_TRUE(OpExtractBasis(schema, { x1, d2 }).IsCorrectlyDefined());
  EXPECT_TRUE(OpExtractBasis(schema, { x2, s1 }).IsCorrectlyDefined());

  schema.Erase(d1);
  EXPECT_FALSE(OpExtractBasis(schema, { d1 }).IsCorrectlyDefined());
}

TEST_F(UTExtractBasis, Execute) {
  EXPECT_TRUE(OpExtractBasis(schema, {}).Execute() == nullptr);

  const auto result = OpExtractBasis(schema, { d3 }).Execute();
  const auto& basis = *result;
  EXPECT_EQ(std::size(basis.Core()), 3U);
  EXPECT_EQ(basis.GetRS(basis.Core().FindAlias("D1").value()).definition, "X1 invalid");
  EXPECT_EQ(basis.GetRS(basis.Core().FindAlias("D2").value()).definition, R"(D1 \setminus D1)"_rs);
}