#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/semantic/RSModel.h"
#include "RSLHelper.hpp"

class UTRSCalculationFacet : public ::testing::Test {
protected:
  using RSModel = ccl::semantic::RSModel;
  using CstType = ccl::semantic::CstType;
  using TextInterpretation = ccl::semantic::TextInterpretation;
  using Factory = ccl::object::Factory;
  using EvalStatus = ccl::semantic::EvalStatus;
  using rsValuesFacet = ccl::semantic::rsValuesFacet;
  using rsCalculationFacet = ccl::semantic::rsCalculationFacet;

protected:
  RSModel model{};
  rsValuesFacet& values{ model.Values() };
  rsCalculationFacet& calc{ model.Calculations() };
  ccl::EntityUID x1, s1;

  void SetupStructure();
};

void UTRSCalculationFacet::SetupStructure() {
  x1 = model.Emplace(CstType::base);
  s1 = model.Emplace(CstType::structured, "B(X1)"_rs);
  values.SetBasicText(x1, TextInterpretation{ { "1", "2", "3" } });
}

TEST_F(UTRSCalculationFacet, Calculate) {
  EXPECT_FALSE(calc.Calculate(x1));

  SetupStructure();
  EXPECT_FALSE(calc.Calculate(x1));
  EXPECT_FALSE(calc.Calculate(s1));

  const auto a1 = model.Emplace(CstType::axiom, "invalid");
  EXPECT_FALSE(calc.Calculate(a1));

  model.SetExpressionFor(a1, "1=1");
  ASSERT_TRUE(calc.Calculate(a1));
  EXPECT_TRUE(values.StatementFor(a1).value());
  ASSERT_TRUE(calc.Calculate(a1));
  EXPECT_TRUE(values.StatementFor(a1).value());

  model.SetExpressionFor(a1, "1=2");
  ASSERT_TRUE(calc.Calculate(a1));
  EXPECT_FALSE(values.StatementFor(a1).value());

  const auto d1 = model.Emplace(CstType::term, R"(X1 \setminus X1)"_rs);
  ASSERT_TRUE(calc.Calculate(d1));
  EXPECT_EQ(values.SDataFor(d1).value(), ccl::object::Factory::EmptySet());
}

TEST_F(UTRSCalculationFacet, RecalculateAll) {
  SetupStructure();
  const auto a1 = model.Emplace(CstType::axiom, "1=1");
  const auto d1 = model.Emplace(CstType::term, R"(X1 \setminus X1)"_rs);

  calc.RecalculateAll();
  const auto a1_val = values.StatementFor(a1);
  ASSERT_TRUE(a1_val.has_value());
  EXPECT_TRUE(a1_val.value());

  const auto d1_val = values.SDataFor(d1);
  ASSERT_TRUE(d1_val.has_value());
  EXPECT_EQ(d1_val.value(), ccl::object::Factory::EmptySet());
}

TEST_F(UTRSCalculationFacet, Count) {
  const auto& constCalc = static_cast<const RSModel&>(model).Calculations();

  EXPECT_EQ(constCalc.Count(EvalStatus::UNKNOWN), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::NEVER_CALCULATED), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::INCALCULABLE), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::AXIOM_FAIL), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::EMPTY), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::HAS_DATA), 0U);

  SetupStructure();
  EXPECT_EQ(constCalc.Count(EvalStatus::UNKNOWN), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::NEVER_CALCULATED), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::INCALCULABLE), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::AXIOM_FAIL), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::EMPTY), 1U);
  EXPECT_EQ(constCalc.Count(EvalStatus::HAS_DATA), 1U);

  model.Emplace(CstType::axiom, "1=1");
  model.Emplace(CstType::axiom, "1=2");
  model.Emplace(CstType::term, R"(X1 \setminus X1)"_rs);
  model.Emplace(CstType::term, R"(BBB(X1 \setminus S1))"_rs);
  EXPECT_EQ(constCalc.Count(EvalStatus::UNKNOWN), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::NEVER_CALCULATED), 4U);
  EXPECT_EQ(constCalc.Count(EvalStatus::INCALCULABLE), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::AXIOM_FAIL), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::EMPTY), 1U);
  EXPECT_EQ(constCalc.Count(EvalStatus::HAS_DATA), 1U);

  calc.RecalculateAll();
  EXPECT_EQ(constCalc.Count(EvalStatus::UNKNOWN), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::NEVER_CALCULATED), 0U);
  EXPECT_EQ(constCalc.Count(EvalStatus::INCALCULABLE), 1U);
  EXPECT_EQ(constCalc.Count(EvalStatus::AXIOM_FAIL), 1U);
  EXPECT_EQ(constCalc.Count(EvalStatus::EMPTY), 2U);
  EXPECT_EQ(constCalc.Count(EvalStatus::HAS_DATA), 2U);
}

TEST_F(UTRSCalculationFacet, WasCalculated) {
  EXPECT_FALSE(calc.WasCalculated(x1));
  SetupStructure();

  EXPECT_FALSE(calc.WasCalculated(x1));
  EXPECT_FALSE(calc.WasCalculated(s1));

  const auto a1 = model.Emplace(CstType::axiom, "1=1");
  const auto d1 = model.Emplace(CstType::term, R"(X1 \setminus X1)"_rs);
  EXPECT_FALSE(calc.WasCalculated(x1));
  EXPECT_FALSE(calc.WasCalculated(s1));
  EXPECT_FALSE(calc.WasCalculated(a1));
  EXPECT_FALSE(calc.WasCalculated(d1));

  calc.Calculate(a1);
  EXPECT_FALSE(calc.WasCalculated(x1));
  EXPECT_FALSE(calc.WasCalculated(s1));
  EXPECT_TRUE(calc.WasCalculated(a1));
  EXPECT_FALSE(calc.WasCalculated(d1));

  calc.Calculate(d1);
  EXPECT_FALSE(calc.WasCalculated(x1));
  EXPECT_FALSE(calc.WasCalculated(s1));
  EXPECT_TRUE(calc.WasCalculated(a1));
  EXPECT_TRUE(calc.WasCalculated(d1));
}

TEST_F(UTRSCalculationFacet, StatusDefault) {
  EXPECT_EQ(calc(ccl::EntityUID{ 42 }), EvalStatus::UNKNOWN);
  EXPECT_FALSE(calc.WasCalculated(ccl::EntityUID{ 42 }));
}

TEST_F(UTRSCalculationFacet, Status) {
  EXPECT_EQ(calc(x1), EvalStatus::UNKNOWN);
  SetupStructure();

  EXPECT_EQ(calc(x1), EvalStatus::HAS_DATA);
  EXPECT_EQ(calc(s1), EvalStatus::EMPTY);

  const auto s2 = model.Emplace(CstType::structured);
  EXPECT_EQ(calc(s2), EvalStatus::UNKNOWN);

  const auto f1 = model.Emplace(CstType::function);
  EXPECT_EQ(calc(f1), EvalStatus::UNKNOWN);

  const auto a1 = model.Emplace(CstType::axiom);
  EXPECT_EQ(calc(a1), EvalStatus::NEVER_CALCULATED);
  model.SetExpressionFor(a1, "1=1");
  EXPECT_EQ(calc(a1), EvalStatus::NEVER_CALCULATED);
  calc.Calculate(a1);
  EXPECT_EQ(calc(a1), EvalStatus::HAS_DATA);

  model.SetExpressionFor(a1, R"(1=2)");
  calc.Calculate(a1);
  EXPECT_EQ(calc(a1), EvalStatus::AXIOM_FAIL);

  model.SetExpressionFor(a1, R"(card(BBB(X1 \setminus S1)) \eq 0)"_rs);
  calc.Calculate(a1);
  EXPECT_EQ(calc(a1), EvalStatus::INCALCULABLE);
  EXPECT_FALSE(values.StatementFor(a1).has_value());

  const auto d1 = model.Emplace(CstType::term);
  EXPECT_EQ(calc(d1), EvalStatus::NEVER_CALCULATED);
  model.SetExpressionFor(d1, R"(X1 \setminus X1)"_rs);
  calc.Calculate(d1);
  EXPECT_EQ(calc(d1), EvalStatus::EMPTY);

  model.SetExpressionFor(d1, R"(X1 \setminus S1)"_rs);
  calc.Calculate(d1);
  EXPECT_EQ(calc(d1), EvalStatus::HAS_DATA);

  model.SetExpressionFor(d1, R"(BBB(X1 \setminus S1))"_rs);
  calc.Calculate(d1);
  EXPECT_EQ(calc(d1), EvalStatus::INCALCULABLE);
  EXPECT_FALSE(values.SDataFor(d1).has_value());

  const auto t1 = model.Emplace(CstType::theorem);
  EXPECT_EQ(calc(t1), EvalStatus::NEVER_CALCULATED);
  model.SetExpressionFor(t1, "1=2");
  calc.Calculate(t1);
  EXPECT_EQ(calc(t1), EvalStatus::HAS_DATA);
}

TEST_F(UTRSCalculationFacet, CalculateResetDependantValue) {
  SetupStructure();
  const auto d1 = model.Emplace(CstType::term, R"(S1 \setminus S1)"_rs);
  const auto structData = Factory::SetV({ 1 });
  values.SetStructureData(s1, structData);
  ASSERT_TRUE(calc.Calculate(d1));
  EXPECT_EQ(calc(d1), EvalStatus::EMPTY);

  values.SetBasicText(x1, TextInterpretation{ { "41" } });
  EXPECT_EQ(calc(d1), EvalStatus::NEVER_CALCULATED);
  EXPECT_EQ(values.SDataFor(s1).value(), structData);
}