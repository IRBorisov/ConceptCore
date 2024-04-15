#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "RSLHelper.hpp"

#include "ccl/ops/RSOperations.h"

class UTRelativation : public ::testing::Test {
protected:
  using EntityUID = ccl::EntityUID;
  using RSForm = ccl::semantic::RSForm;
  using CstType = ccl::semantic::CstType;
  using OpRelativation = ccl::ops::OpRelativation;
  using Token = ccl::rslang::Token;
  using TokenID = ccl::rslang::TokenID;

protected:
  RSForm schema{};

  EntityUID x1, x2, s1;

  void SetupMinimalSchema();
};

void UTRelativation::SetupMinimalSchema() {
  x1 = schema.Emplace(CstType::base);
  x2 = schema.Emplace(CstType::base);
  s1 = schema.Emplace(CstType::structured, "B(X1*X2)"_rs);

  schema.SetConventionFor(x1, "X1");
  schema.SetConventionFor(x2, "X2");
  schema.SetConventionFor(s1, "S1");
}

TEST_F(UTRelativation, IsCorrectlyDefined) {
  EXPECT_FALSE(OpRelativation::IsCorrectlyDefined(schema, EntityUID{ 42 }));
  EXPECT_TRUE(OpRelativation(schema, EntityUID{ 42 }).Execute() == nullptr);

  s1 = schema.Emplace(CstType::structured, "invalid");
  EXPECT_FALSE(OpRelativation::IsCorrectlyDefined(schema, s1));

  schema.SetExpressionFor(s1, "B(X1)"_rs);
  EXPECT_FALSE(OpRelativation::IsCorrectlyDefined(schema, s1, { EntityUID{ s1 + 1 } }));

  x1 = schema.Emplace(CstType::base);
  EXPECT_TRUE(OpRelativation::IsCorrectlyDefined(schema, s1));
  EXPECT_FALSE(OpRelativation::IsCorrectlyDefined(schema, s1, { s1 }));
  EXPECT_FALSE(OpRelativation::IsCorrectlyDefined(schema, s1, { EntityUID{ s1 + 1 } }));

  x2 = schema.Emplace(CstType::base);
  EXPECT_TRUE(OpRelativation::IsCorrectlyDefined(schema, s1, { x2 }));

  const auto d1 = schema.Emplace(CstType::term, R"(X1 \setminus X1)"_rs);
  EXPECT_TRUE(OpRelativation::IsCorrectlyDefined(schema, s1, { d1 }));
  EXPECT_FALSE(OpRelativation::IsCorrectlyDefined(schema, d1));
}

TEST_F(UTRelativation, ExecuteTargetStruct) {
  SetupMinimalSchema();
  const auto opReturn = OpRelativation(schema, s1, { x1, x2 }).Execute();
  ASSERT_TRUE(opReturn != nullptr);
  const auto& newSchema = *opReturn;
  EXPECT_EQ(std::ssize(schema.Core()), std::ssize(newSchema.Core()));
  EXPECT_EQ(newSchema.GetRS(s1).definition, "BB(X1*X2)"_rs);
}

TEST_F(UTRelativation, ExecuteTargetBase) {
  SetupMinimalSchema();
  const auto opReturn = OpRelativation(schema, x1, { x2 }).Execute();
  ASSERT_TRUE(opReturn != nullptr);
  const auto& newSchema = *opReturn;
  EXPECT_EQ(std::ssize(schema.Core()) + 2, std::ssize(newSchema.Core()));
  EXPECT_EQ(newSchema.GetRS(s1).definition, "B(S2*B(S2*X2))"_rs);
}

TEST_F(UTRelativation, ExecutePromoteUnrelatedBase) {
  SetupMinimalSchema();
  const auto x3 = schema.Emplace(CstType::base);
  schema.SetDefinitionFor(x3, "X3");
  const auto opReturn = OpRelativation(schema, s1, { x2, x1 }).Execute();
  ASSERT_TRUE(opReturn != nullptr);

  const auto& newSchema = *opReturn;
  EXPECT_FALSE(newSchema.Contains(x3));

  const auto newX3 = newSchema.Core().FindAlias("X3").value();
  EXPECT_EQ(newSchema.GetText(newX3).definition.Raw(), "X3");
  const auto newS2 = newSchema.Core().FindAlias("S2").value();
  EXPECT_EQ(newSchema.GetRS(newS2).definition, "B(S1*B(X3))"_rs);
}

TEST_F(UTRelativation, ExecuteModifyTerm) {
  SetupMinimalSchema();

  const auto d1 = schema.Emplace(CstType::term, R"(X1 \setminus X1)"_rs);
  const auto d2 = schema.Emplace(CstType::term, R"(D1 \setminus X1)"_rs);
  const auto opReturn = OpRelativation(schema, x1, { x2, s1 }).Execute();
  ASSERT_TRUE(opReturn != nullptr);
  const auto& newSchema = *opReturn;
  ASSERT_TRUE(newSchema.Contains(d1));
  EXPECT_EQ(newSchema.GetRS(d1).definition,
            R"(I{(__base1, __rt1) | __base1 \from S2; __rt1 \assign __base1 \setminus __base1})"_rs);
  EXPECT_EQ(newSchema.GetRS(d2).definition,
            R"(I{(__base1, __rt1) | __base1 \from S2; __rt1 \assign F1[__base1, D1] \setminus __base1})"_rs);
  EXPECT_EQ(newSchema.GetRS(newSchema.Core().FindAlias("F1").value()).definition, 
            "[" + R"(__base1 \in B(X1))"_rs + ", " + R"(__term \in B(B(X1)*R1))"_rs + "] " +
            R"(debool(Pr2(Fi1[{__base1}](__term))))"_rs);
}

TEST_F(UTRelativation, ExecuteModifyAxiom) {
  const auto expectedExpr1 = R"(\A __base1 \in S1 (Pr1(__base1) \eq X1 \and 1 \eq 1))"_rs;
  const auto expectedExpr2 = R"(\A __base1 \in S1 \A a \in __base1 a \eq a)"_rs;

  SetupMinimalSchema();

  const auto a1 = schema.Emplace(CstType::axiom, R"(Pr1(S1) \eq X1 \and 1 \eq 1)"_rs);
  const auto a2 = schema.Emplace(CstType::axiom, R"(\A a \in S1 a \eq a)"_rs);
  ASSERT_EQ(schema.GetParse(a1).status, ccl::semantic::ParsingStatus::VERIFIED);
  ASSERT_EQ(schema.GetParse(a2).status, ccl::semantic::ParsingStatus::VERIFIED);

  const auto opReturn = OpRelativation(schema, s1, { x1, x2 }).Execute();
  ASSERT_TRUE(opReturn != nullptr);
  const auto& newSchema = *opReturn;
  ASSERT_TRUE(newSchema.Contains(a1));
  ASSERT_TRUE(newSchema.Contains(a2));
  EXPECT_EQ(newSchema.GetRS(a1).definition, expectedExpr1);
  EXPECT_EQ(newSchema.GetRS(a2).definition, expectedExpr2);
}

TEST_F(UTRelativation, ExecuteModifyFunction) {
  const auto expectedFunc = R"([__base1 \in B(X1), a \in B(__base1)] a \setminus __base1)"_rs;
  const auto expectedTerm = R"(I{(__base1, __rt1) | __base1 \from S2; __rt1 \assign F1[__base1, F1[__base1, __base1]]})"_rs;
  
  SetupMinimalSchema();

  const auto f1 = schema.Emplace(CstType::function, R"([a \in B(X1)] a \setminus X1)"_rs);
  const auto d1 = schema.Emplace(CstType::term, "F1[F1[X1]]");
  ASSERT_EQ(schema.GetParse(f1).status, ccl::semantic::ParsingStatus::VERIFIED);
  ASSERT_EQ(schema.GetParse(d1).status, ccl::semantic::ParsingStatus::VERIFIED);

  const auto opReturn = OpRelativation(schema, x1, { s1, x2 }).Execute();
  ASSERT_TRUE(opReturn != nullptr);
  const auto& newSchema = *opReturn;
  ASSERT_TRUE(newSchema.Contains(f1));
  EXPECT_EQ(newSchema.GetRS(f1).definition, expectedFunc);
  EXPECT_EQ(newSchema.GetRS(d1).definition, expectedTerm);
}

TEST_F(UTRelativation, ExecuteKeepIgnored) {
  SetupMinimalSchema();
  const auto x3 = schema.Emplace(CstType::base);
  const auto d1 = schema.Emplace(CstType::term, R"(S1 \setminus S1)"_rs);
  const auto d2 = schema.Emplace(CstType::term, R"(X1 \setminus X1)"_rs);
  const auto d3 = schema.Emplace(CstType::term, R"(X3 \setminus X3)"_rs);

  const auto opReturn = OpRelativation(schema, x1, { s1, x2, x3, d1, d2 }).Execute();
  ASSERT_TRUE(opReturn != nullptr);
  const auto& newSchema = *opReturn;
  EXPECT_EQ(newSchema.GetRS(d1).definition, R"(S1 \setminus S1)"_rs);
  EXPECT_EQ(newSchema.GetRS(d2).definition, R"(S2 \setminus S2)"_rs);
  EXPECT_EQ(newSchema.GetRS(d3).definition, R"(X3 \setminus X3)"_rs);
}