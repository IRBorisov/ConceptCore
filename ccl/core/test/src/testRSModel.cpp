#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/semantic/RSModel.h"
#include "ccl/semantic/RSForm.h"
#include "RSLHelper.hpp"

class UTRSModel : public ::testing::Test {
protected:
  using RSModel = ccl::semantic::RSModel;
  using CstType = ccl::semantic::CstType;
  using ParsingStatus = ccl::semantic::ParsingStatus;
  using TextInterpretation = ccl::semantic::TextInterpretation;
  using StructuredData = ccl::object::StructuredData;
  using RSForm = ccl::semantic::RSForm;
  using Factory = ccl::object::Factory;

protected:
  RSModel model{};
  ccl::semantic::rsValuesFacet& values{ model.Values() };
  ccl::semantic::rsCalculationFacet& calc{ model.Calculations() };
  ccl::EntityUID x1{ model.Emplace(CstType::base) };
};

TEST_F(UTRSModel, EraseData) {
  model.Erase(x1);
  const auto s1 = model.Emplace(CstType::structured);
  EXPECT_FALSE(model.Values().TextFor(x1) != nullptr);
  EXPECT_FALSE(model.Values().SDataFor(x1).has_value());
  EXPECT_FALSE(model.Values().SDataFor(s1).has_value());
}

TEST_F(UTRSModel, SetExpression) {
  EXPECT_FALSE(model.SetExpressionFor(ccl::EntityUID{ x1 + 1 }, {}));

  const auto s1 = model.Emplace(CstType::structured);
  const auto d1 = model.Emplace(CstType::term);
  EXPECT_FALSE(model.Values().SDataFor(s1).has_value());
  EXPECT_FALSE(model.Values().SDataFor(d1).has_value());

  model.SetExpressionFor(d1, R"(X1 \setminus X1)"_rs);
  model.SetExpressionFor(s1, "B(X1)"_rs);
  ASSERT_TRUE(model.Values().SDataFor(s1).has_value());
  EXPECT_EQ(model.Values().SDataFor(s1).value(), Factory::EmptySet());
  EXPECT_FALSE(model.Values().SDataFor(d1).has_value());

  model.SetExpressionFor(s1, "X1");
  ASSERT_EQ(model.GetParse(s1).status, ParsingStatus::VERIFIED);
  EXPECT_FALSE(model.Values().SDataFor(s1).has_value());

  model.SetExpressionFor(s1, "X1*X1"_rs);
  ASSERT_EQ(model.GetParse(s1).status, ParsingStatus::VERIFIED);
  EXPECT_FALSE(model.Values().SDataFor(s1).has_value());
}

TEST_F(UTRSModel, OnExpressionChangeResetData) {
  values.SetBasicText(x1, TextInterpretation{ { "1" } });
  const auto structData = ccl::object::Factory::Set({ ccl::object::Factory::Val(1) });
}